#include "peermr.hh"

namespace eclipse {
// Constructor{{{
PeerMR::PeerMR (Context& c) : Peer (c) {
}

PeerMR::~PeerMR () {

}
// }}} 
// insert {{{
bool PeerMR::insert (std::string key, std::string val) {
  if (exists(key)) {
    auto value = cache->get(key);
    value += string("\n") + val;
    cache->put(key, value);

  } else {
    insert(key, val);
  }

  return true;
}
// }}}
// lookup {{{
void PeerMR::lookup (std::string key, req_func f) {
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
} /* eclipse  */ 