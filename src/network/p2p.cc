#include "p2p.hh"

using namespace eclipse::network;

// constructor {{{
P2P::P2P(tcp::socket* a, tcp::socket* b, Context& c, AsyncNode* node_) : 
  AsyncChannel(a, b, c, node_)
{ }
// }}}
// is_multiple {{{
bool P2P::is_multiple () { return true; }
// }}}
