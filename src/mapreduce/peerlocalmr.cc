#include "peerlocalmr.hh"

namespace eclipse {
// Constructor{{{
PeerLocalMR::PeerLocalMR (Context& c) : PeerLocal(c) {
}

PeerLocalMR::~PeerLocalMR () {

}
// }}} 
// insert {{{
bool PeerLocalMR::insert (std::string key, std::string val) {
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
void PeerLocalMR::lookup (std::string key, req_func f) {
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
