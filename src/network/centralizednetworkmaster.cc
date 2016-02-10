#include "centralizednetwork.hh"

namespace eclipse {
namespace network {
// Constructors {{{
CentralizedNetworkMaster::CentralizedNetworkMaster () {
  Settings& setted = c.settings;
  vec_str nodes  = setted.get<vec_str> ("network.nodes");

  client = new Channel();
  topology = new CentralizedTopology (c, client, node);
}
CentralizedNetworkMaster::~CentralizedNetworkMaster () { }
// }}}
// establish {{{
bool CentralizedNetworkMaster::establish () {
  topology->establish();
  return true;
}
// }}}
// close {{{
bool CentralizedNetworkMaster::close () {
  return true;
}
// }}}
// size {{{
size_t CentralizedNetworkMaster::size () {
  return 1;
}
// }}}
// send {{{
bool CentralizedNetworkMaster::send (int, messages::Message*) {
  client->do_write(m);
  return true;
}
// }}}
}
} /* eclipse  */ 
