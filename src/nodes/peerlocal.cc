// includes & usings {{{
#include "peerlocal.hh"
#include "peerremote.hh"
#include "noderemote.hh"
#include "../common/settings.hh"
#include "../common/definitions.hh"
#include "../messages/factory.hh"
#include "../messages/boost_impl.hh"
#include "../network/meshtopology.hh"
#include "../network/ringtopology.hh"

#include <boost/range/adaptor/map.hpp>
#include <boost/asio.hpp>
#include <algorithm>
#include <iterator>
#include <memory>

using namespace eclipse;
using namespace eclipse::messages;
using namespace boost::asio;
using namespace std;

using vec_str    = std::vector<std::string>;
using vec_node   = std::vector<NodeRemote*>;
using PAIR       = std::pair<int, NodeRemote*>;
// }}}

namespace eclipse {
// Auxiliar functions {{{
static auto range_of = [] (multimap<int, NodeRemote* >& m, int type) -> vec_node {
  auto it = m.equal_range (type);
  vector<NodeRemote*> vec;

  std::transform (it.first, it.second, back_inserter(vec), [] (PAIR p) {
    return p.second;
  });
  return vec;
};

auto corresponding_node = [] (string k, int  size) -> int {
   return h(k.c_str(), k.length()) % size;
};
// }}}
// Constructor & destructor {{{
PeerLocal::PeerLocal(Settings& setted) : NodeLocal(setted) { 
  setted.load();

  int numbin     = setted.get<int> ("cache.numbin");
  int cachesize  = setted.get<int> ("cache.size");
  string tType   = setted.get<string> ("network.topology");
  vec_str nodes  = setted.get<vec_str> ("network.nodes");
  concurrency    = setted.get<int> ("cache.concurrency");

  histogram = make_unique<Histogram> (nodes.size(), numbin);
  cache     = make_unique<lru_cache<string, string>> (cachesize);

  int i = 0;
  for (auto& node : nodes) {
    if (node == ip_of_this) 
      id = i;
    universe.insert ({PEER, new PeerRemote (this, i++)});
  }

  // topology initialization
  if (tType == "mesh") {
    topology = make_unique<MeshTopology>
      (io_service, logger.get(), nodes, port, id);
  
  } else if (tType == "ring") {
    topology = make_unique<RingTopology> (io_service, logger.get(), nodes, port, id);
  }
}

PeerLocal::~PeerLocal() {
  for (auto& t : threads) t->join();
}
// }}}
// establish {{{
bool PeerLocal::establish () {
  int i = 0;

  logger->info ("Running Eclipse id=%d ip=%s size=%d", 
      id, ip_of_this.c_str(), universe.size());
  topology->establish();

  for (auto& node : range_of(universe, PEER)) {
    if (i != id)  {
      node->set_channel (topology->get_channel(i));
    } 
    i++;
    if (node->get_id() == 23) continue;
  }

 return true;
}
// }}}
// insert {{{
void PeerLocal::insert (std::string k, std::string v) {

  while (not topology->is_online() ) sleep(1);

  int i = 0;
  for (auto& node : range_of(universe, PEER)) {
    if (i != id)  {
      node->start();
    }
    i++;
  }
  int idx = corresponding_node (k, universe.size());

  logger->info ("Inserting [%10s]:[%10s] -> %d", k.c_str(),v.c_str(), idx);
  if (idx == this->id) {
    cache->put (k, v);

  } else {
    auto peer = range_of (universe, PEER)[idx]; 
    auto msg  = new KeyValue (k, v);
    peer->do_write (msg);
  }
}
// }}}
// lookup {{{
std::string PeerLocal::request (std::string key) {
 int idx = corresponding_node(key, universe.size());
 if (idx == this->id) {
   return cache->get (key);

 } else {
   auto peer = range_of (universe, PEER)[idx];
   KeyRequest k_req (key);
   k_req.set_origin (id);
   peer->do_write (&k_req);
   return "WAIT";
 }
}
// }}}
// exist {{{
bool PeerLocal::exists (std::string key) {
  return cache->exists (key);
}
// }}}
// process_message (Boundaries* m) {{{
template<> void PeerLocal::process_message (Boundaries* m) {
  *histogram << *m;

  int dest = m->get_destination();
  if (dest != this->id) {
      //(*it)->send(m);
  }
}
// }}}
// process_message (KeyValue* m) {{{
template<> void PeerLocal::process_message (KeyValue* m) {
  string& key = m->key;

  int which_node = corresponding_node (key, universe.size());
  if (which_node == id) {
    logger->info ("Instering key = %s", key.c_str());
    histogram->count_query(which_node);
    //histogram->updateboundary();
    cache->put (key, m->value);

  } else {
    auto peers = range_of (universe, PEER);
    auto it = find_if (peers.begin(), peers.end(), [&which_node] (NodeRemote* n) {
        return n->get_id() == which_node;
        });

    if (it != peers.end()) {
      //(*it)->send(m);
    }
  }
}
// }}}
// process_message (KeyRequest* m) {{{
template<> void PeerLocal::process_message (KeyRequest* m) {
  string& key = m->key;
  string value;
  if (cache->exists(key)) {
    value = cache->get(key);
  } else {
    value = "EMPTY";
  }

  KeyValue kv (key, value);
  auto* node = range_of(universe, PEER)[m->origin];
  node->do_write(&kv);
}
// }}}
// process_message (Control* m) {{{
template<> void PeerLocal::process_message (Control* m) {
  switch (m->type) {
    case messages::SHUTDOWN:
      this->close();
      break;

    case messages::RESTART:
      break;

      //    case PING:
      //      process_ping (m);
      //      break;
  }
}
// }}}
// process_message (Message*) {{{
template<> void PeerLocal::process_message (Message* m) {
  string type = m->get_type();

  if (type == "Boundaries") {
    auto m_ = dynamic_cast<Boundaries*>(m);
    process_message(m_);

  } else if (type == "KeyValue") {
    auto m_ = dynamic_cast<KeyValue*>(m);
    process_message(m_);

  } else if (type == "Control") {
    auto m_ = dynamic_cast<Control*>(m);
    process_message(m_);

  } else if (type == "KeyRequest") {
    auto m_ = dynamic_cast<KeyRequest*>(m);
    process_message(m_);
  }
}
// }}}
// run {{{
void PeerLocal::run () {
  for (int i = 0; i < concurrency; i++ ) {
    auto t = new std::thread ( [this] {
        this->io_service.run();
        });

    threads.emplace_back (t);
  }
}
// }}}
// join {{{
void PeerLocal::join () {
  for (auto& t : threads) t->join();
}
// }}}
// close {{{
void PeerLocal::close() { exit(EXIT_SUCCESS); }
// }}}
}
