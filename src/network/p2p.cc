#include "p2p.hh"

using namespace eclipse::network;

// constructor {{{
P2P::P2P(tcp::socket* a, tcp::socket* b, NetObserver* node_, int i) : 
  AsyncChannel(a, b, node_, i)
{ }
// }}}
// is_multiple {{{
bool P2P::is_multiple () { return true; }
// }}}
