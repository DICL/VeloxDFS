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
  if (key )
}
// }}}
// lookup {{{
std::string PeerLocalMR::lookup (std::string key) {
 string value;
 if (cache->exists(key)) {
   value = cache->lookup(key);
 
 } else {
   // Read file from input folder 
 }
}
// }}}
} /* eclipse  */ 
