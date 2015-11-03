#include "peerremote.hh"
#include "../messages/factory.hh"
#include <string>

using namespace network;
using namespace std;

namespace Nodes {
// Constructor & destructor {{{
PeerRemote::PeerRemote (io_ptr& io, string host_, int p, int id) : 
  NodeRemote(io, host_, p, id) 
{

}
// 
PeerRemote::~PeerRemote () { }
// }}}
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
//za on_write_lookup {{{
//zzaavoid PeerRemote::on_write_lookup (.. , .. ) {


//}
// }}}

void PeerRemote::send(Message* m) {
  string tosend;
  tosend << m;
  socket->send (boost::asio::buffer(tosend));
}
} /* nodes */
