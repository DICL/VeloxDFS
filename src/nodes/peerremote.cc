#include "peerremote.hh"
#include "../messages/factory.hh"
#include <string>

using namespace eclipse::messages;
using namespace std;
using namespace std::placeholders;

namespace eclipse {
// on_connect {{{
bool NodeRemote::on_connect(boost::system::error& ec) {
  if (!ec) {
    do_read();
  }
}
// }}}
// do_read {{{
void PeerRemote::do_read () {
  async_read (*socket, buffer(data), bind(&PeerRemote::on_read, this, _1, _2));
}
// }}}
// on_read {{{
void PeerRemote::on_read (boost::system::error_code& ec, size_t s) {
  if (!ec) {
  
  }
}
// }}}
// do_write {{{
void PeerRemote::do_write (Message* m) {

}
// }}}
// on_read {{{
void PeerRemote::on_write (boost::system::error_code& ec, size_t s) {

}
// }}}
// send {{{
void PeerRemote::send(Message* m) {
  string tosend;
  tosend << m;
  socket->send (boost::asio::buffer(tosend));
}
// }}}
} /* nodes */
