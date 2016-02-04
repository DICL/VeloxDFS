#include "peerlocalmr.hh"

namespace eclipse {
// Constructor{{{
PeerLocalMR::PeerLocalMR (Context& c) : PeerLocal(c) {
}

PeerLocalMR::~PeerLocalMR () {

}
// }}} 
// insert {{{
bool PeerLocalMR::insert (std::string, std::string) {
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
// insert_cache {{{
bool PeerLocalMR::insert_cache (std::string, std::string) {
}
// }}}
// insert_disk {{{
bool PeerLocalMR::insert_disk (std::string, std::string) {
}
// }}}
// lookup_cache {{{
bool PeerLocalMR::lookup_cache (std::string) {
}
// }}}
// lookup_disk {{{
bool PeerLocalMR::lookup_disk (std::string) {
}
// }}}
} /* eclipse  */ 
