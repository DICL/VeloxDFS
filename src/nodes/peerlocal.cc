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

  if (tType == "mesh") {
    topology = make_unique<MeshTopology>
      (io_service, logger.get(), nodes, port, id);
  
  } else if (tType == "ring") {
    topology = make_unique<RingTopology>
      (io_service, logger.get(), nodes, port, id);
  }
}

PeerLocal::~PeerLocal() {
  //for (auto t : threads)
  //  t->join();
}
// }}}
// establish {{{
bool PeerLocal::establish () {
  int i = 0;

  logger->info ("Running Eclipse network id=%d ip=%s", 
      id, ip_of_this.c_str());
  topology->establish();
  for (auto node : range_of(universe, PEER)) {
    if (i == id) continue;
    node->set_channel (topology->get_channel(i++));
  }

 // while (not topology->is_online()) 
 //   sleep(1); 
 //
 return true;
}
// }}}
// insert {{{
void PeerLocal::insert (std::string k, std::string v) {
  //int idx = correspoding_node(k);
  //if (idx == id_of_this) {
  //  cache.insert (k, v);

  //} else {
  //  PeerRemote& peer = range_of(universe, PEER)[idx]; 
  //    peer->insert(k, v);
  //}
}
// }}}
// lookup {{{
std::string PeerLocal::lookup (std::string s) {
  //int idx = correspoding_node(key);
  //string value;
  //if (idx == id_of_this) {
  //  value = cache.get (k);

  //} else {
  //  PeerRemote& peer = get_peers_of(universe)[idx]
  //    value = peer->lookup_s (k);
  //}
  //return value;
  return "void";
}
// }}}
// exist {{{
bool PeerLocal::exist (std::string) {
 return true;
}
// }}}
// close {{{
void PeerLocal::close() { exit(EXIT_SUCCESS); }
// }}}
// process_message (Boundaries* m) {{{
template<> void PeerLocal::process_message (Boundaries* m) {
  *histogram << *m;

  string dest = m->get_destination();
  if (dest != ip_of_this) {
    auto peers = range_of (universe, PEER);
    auto it = find_if (peers.begin(), peers.end(), [&dest] (NodeRemote* n) {
        return n->get_ip() == dest;
        });

    if (it != peers.end()) {
      //(*it)->send(m);
    }
  }
}
// }}}
// process_message (KeyValue* m) {{{
template<> void PeerLocal::process_message (KeyValue* m) {
  string& key = m->key;

  int which_node = 0;  //corresponding_node(key);
  if (which_node == id) {
    histogram->count_query(which_node);
    histogram->updateboundary();
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
    Control* m_ = dynamic_cast<Control*>(m);
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
}
