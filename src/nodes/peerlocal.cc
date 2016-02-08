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
// Constructor & destructor {{{
PeerLocal::PeerLocal(Context& context) : NodeLocal(context) { 
  Settings& setted = context.settings;

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
    universe.insert ({i, new PeerRemote (this, i)});
    i++;
  }

  // topology initialization
  if (tType == "mesh") {
    topology = make_unique<MeshTopology>
      (io_service, logger, nodes, port, id);
  
  } else if (tType == "ring") {
    topology = make_unique<RingTopology> (io_service, logger, nodes, port, id);
  }
}

PeerLocal::~PeerLocal() {
  for (auto& t : threads) t->join();
}
// }}}
// H {{{
int PeerLocal::H(string k) {
  uint32_t index = h(k.c_str(), k.length());
  return histogram->get_index(index);
}
// }}}
// establish {{{
bool PeerLocal::establish () {
  logger->info ("Running Eclipse id=%d ip=%s size=%d", 
      id, ip_of_this.c_str(), universe.size());
  topology->establish();

  for (unsigned int i = 0; i < universe.size(); i++ )
    if (i != static_cast<unsigned int> (id))  {
      auto channel  = topology->get_channel(i);
      auto node = universe[i];
      channel->set_node(node);
      node->set_channel (channel);
    } 

 return true;
}
// }}}
// insert {{{
void PeerLocal::insert (std::string k, std::string v) {
  int idx = H(k);
  logger->info ("Inserting [%10s]:[%10s] -> %d", k.c_str(),v.c_str(), idx);

  if (belongs(k)){
    cache->put (k, v);

  } else {
    auto peer = universe[idx]; 
    auto msg  = new KeyValue (k, v);
    peer->do_write (msg);
  }
}
// }}}
// request {{{
void PeerLocal::request (std::string key, req_func f) {
 int idx = H(key);
 if (idx != id) {
   auto peer = universe[idx];
   KeyRequest k_req (key);
   k_req.set_origin (id);
   peer->do_write (&k_req);
   requested_blocks.insert ({key, f});
 }
}
// }}}
// exist {{{
bool PeerLocal::exists (std::string key) {
  return cache->exists (key);
}
// }}}
// belongs {{{
bool PeerLocal::belongs (std::string key) {
  return H(key) == id;
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
// process (Boundaries* m) {{{
template<> void PeerLocal::process (Boundaries* m) {
  *histogram << *m;

  int dest = m->get_destination();
  if (dest != this->id) {
      //(*it)->send(m);
  }
}
// }}}
// process (KeyValue* m) {{{
template<> void PeerLocal::process (KeyValue* m) {
  string& key = m->key;

  int which_node = H(key);
  if (which_node == id or m->destination == id)  {
    logger->info ("Instering key = %s", key.c_str());
    histogram->count_query(which_node);
    //histogram->updateboundary();
    cache->put (key, m->value);

    if (requested_blocks.find(key) !=requested_blocks.end()){
      logger->info ("Executing func");
      requested_blocks[key](m->value);
      requested_blocks.erase(key);
    }

  } 
}
// }}}
// process (KeyRequest* m) {{{
template<> void PeerLocal::process (KeyRequest* m) {
  logger->info ("Arrived req key = %s", m->key.c_str());
  string& key = m->key;
  string value;
  if (cache->exists(key)) {
    value = cache->get(key);
  } else {
    value = "EMPTY";
  }

  KeyValue kv (key, value);
  kv.destination = m->origin;
  auto* node = universe[m->origin];
  node->do_write(&kv);
}
// }}}
// process (Control* m) {{{
template<> void PeerLocal::process (Control* m) {
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
void PeerLocal::process_message (Message* m) {
  string type = m->get_type();

  if (type == "Boundaries") {
    auto m_ = dynamic_cast<Boundaries*>(m);
    process(m_);

  } else if (type == "KeyValue") {
    auto m_ = dynamic_cast<KeyValue*>(m);
    process(m_);

  } else if (type == "Control") {
    auto m_ = dynamic_cast<Control*>(m);
    process(m_);

  } else if (type == "KeyRequest") {
    auto m_ = dynamic_cast<KeyRequest*>(m);
    process(m_);
  }
}
// }}}
}
