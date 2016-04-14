#include "server.hh"

using namespace eclipse::network;

// constructor {{{
Server::Server(tcp::socket*, tcp::socket* s, NetObserver* node_, int i) : 
  AsyncChannel(s, s, node_, i)
{ }
// }}}
// is_multiple {{{
bool Server::is_multiple () { return false; }
// }}}
