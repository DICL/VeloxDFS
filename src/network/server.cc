#include "server.hh"

using namespace eclipse::network;
using namespace std;

// constructor {{{
Server::Server(NetObserver* node_) :
  AsyncChannel(node_)
{ 
}
// }}}
// is_multiple {{{
bool Server::is_multiple () { return false; }
// }}}
