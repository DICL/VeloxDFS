// includes & usings {{{
#include "peer.hh"
#include "../network/asyncnetwork.hh"
#include "../network/p2p.hh"
#include "../common/settings.hh"
#include "../common/definitions.hh"
#include "../messages/factory.hh"
#include "../messages/boost_impl.hh"

#include <boost/range/adaptor/map.hpp>
#include <boost/asio.hpp>
#include <algorithm>
#include <iterator>
#include <memory>

using namespace eclipse;
using namespace eclipse::messages;
using namespace eclipse::network;
using namespace boost::asio;
using namespace std;

using vec_str    = std::vector<std::string>;
// }}}

namespace eclipse {
// Constructor & destructor {{{
Peer::Peer (Context& context) : Node (context) { 
  Settings& setted = context.settings;

  int numbin     = setted.get<int> ("cache.numbin");
  int cachesize  = setted.get<int> ("cache.size");
  int port       = setted.get<int>("network.port_cache");

  network   = new AsyncNetwork<P2P>(this, context, 10, port);
  histogram = make_unique<Histogram> (network->size(), numbin);
  cache     = make_unique<lru_cache<string, string>> (cachesize);
}

Peer::~Peer() { }
// }}}
// H {{{
int Peer::H(string k) {
  uint32_t index = h(k.c_str(), k.length());
  return histogram->get_index(index);
}
// }}}
// establish {{{
bool Peer::establish () {
 logger->info ("Running Eclipse id=%d", id);
 network->establish();

 while (not connected) sleep(1);
 return true;
}
// }}}
// insert {{{
void Peer::insert (std::string k, std::string v) {
  int idx = H(k);
  logger->info ("Inserting [%10s]:[%10s] -> %d", k.c_str(),v.c_str(), idx);

  if (belongs(k)){
    cache->put (k, v);

  } else {
    auto msg  = new KeyValue (k, v);
    network->send(idx, msg);
  }
}
// }}}
// request {{{
void Peer::request (std::string key, req_func f) {
 int idx = H(key);
 if (idx != id) {
   KeyRequest k_req (key);
   k_req.set_origin (id);
   network->send (idx, &k_req);
   requested_blocks.insert ({key, f});
 }
}
// }}}
// exist {{{
bool Peer::exists (std::string key) {
  return cache->exists (key);
}
// }}}
// belongs {{{
bool Peer::belongs (std::string key) {
  return H(key) == id;
}
// }}}
// close {{{
void Peer::close() { exit(EXIT_SUCCESS); }
// }}}
// process (Boundaries* m) {{{
template<> void Peer::process (Boundaries* m) {
  *histogram << *m;

  int dest = m->get_destination();
  if (dest != this->id) {
      //(*it)->send(m);
  }
}
// }}}
// process (KeyValue* m) {{{
template<> void Peer::process (KeyValue* m) {
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
template<> void Peer::process (KeyRequest* m) {
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
  network->send(m->origin, &kv);
}
// }}}
// process (Control* m) {{{
template<> void Peer::process (Control* m) {
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
// on_read (Message*) {{{
void Peer::on_read (Message* m) {
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
// on_connect {{{
void Peer::on_connect () {
  connected = true;
  logger->info ("Network established id=%d", id);
}
// }}}
// on_disconnect {{{
void Peer::on_disconnect () {

}
// }}}
}
