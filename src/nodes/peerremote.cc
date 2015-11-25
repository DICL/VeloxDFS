#include "peerremote.hh"
#include "../messages/factory.hh"
#include <string>

using namespace network;
using namespace std;

namespace Nodes {
// insert {{{
//void PeerRemote::insert (string k, string v) {
  //Serialize Key/value
  //send
//  async_write (*socket, buffer(data), bind(&PeerRemote::on_write, this, _1, _2));
//}
// }}}
// on_write_insertion {{{
//void PeerRemote::on_write_insertion (.. , ..) {
  //Dumb
//}
// }}}
// on_write_lookup {{{
// void PeerRemote::on_write_lookup (.. , .. ) {


//}
// }}}
// send {{{
void PeerRemote::send(Message* m) {
  string tosend;
  tosend << m;
  socket->send (boost::asio::buffer(tosend));
}
// }}}
} /* nodes */
