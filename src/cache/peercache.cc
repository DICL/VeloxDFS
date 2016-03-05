#include "peermr.hh"

namespace eclipse {
// Constructor{{{
PeerCache::PeerCache (Context& c) : Peer (c) {
  histogram = make_unique<Histogram> (setted.get<vec_str>("network.nodes").size(), 
      numbin);
  cache     = make_unique<lru_cache<string, string>> (cachesize);
}

PeerCache::~PeerCache () {

}
// }}} 
// H {{{
int PeerDFS::H(string k) {
  uint32_t index = h(k);
  return histogram->get_index(index);
}
// }}}
// belongs {{{
bool PeerDFS::belongs (std::string key) {
  return H(key) == id;
}
// }}}
// insert {{{
void PeerDFS::insert (std::string k, std::string v) {
  int idx = H(k);
  logger->info ("Inserting [%10s][H:%u] -> %d", k.c_str(), h(k), idx);

  if (belongs(k)){
    cache->put (k, v);

  } else {
    auto msg  = new KeyValue (k, v);
    network->send(idx, msg);
  }
}
// }}}
// request {{{
void PeerDFS::request (std::string key, req_func f) {
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
bool PeerDFS::exists (std::string key) {
  return cache->exists (key);
}
// }}}
// lookup {{{
void PeerCache::lookup (std::string key, req_func f) {
 if (exists(key)) {
   string value = cache->get(key);
   f(value);
 
 } else {
   if (belongs(key)) {
     //Read from disk
   } else {
     request(key, f);
   }
 }
}
// }}}
// info {{{
vec_str PeerDFS::info() {
  return cache->dump_keys();
}
// }}}
// process (Boundaries* m) {{{
template<> void PeerDFS::process (Boundaries* m) {
  *histogram << *m;

  int dest = m->get_destination();
  if (dest != this->id) {
      //(*it)->send(m);
  }
}
// }}}
} /* eclipse  */ 
