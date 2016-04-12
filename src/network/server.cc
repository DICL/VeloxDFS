#include "server.hh"

using namespace eclipse::network;

// constructor {{{
Server::Server(tcp::socket*, tcp::socket* s, Context& c, AsyncNode* node_) : 
  AsyncChannel(s, s, c, node_)
{ }
// }}}
// is_multiple {{{
bool Server::is_multiple () { return false; }
// }}}
