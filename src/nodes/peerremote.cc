#include "peerremote.hh"
#include "../messages/factory.hh"
#include <string>
#include <boost/bind.hpp>

using namespace std;
using namespace std::placeholders;
namespace ph = boost::asio::placeholders;

namespace eclipse {
// constructor {{{
PeerRemote::PeerRemote(NodeLocal* p, int i) : NodeRemote(p, i) {
 owner_peer = dynamic_cast<PeerLocal*>(p);
}
// }}}
// on_connect {{{
void PeerRemote::on_connect(const boost::system::error_code& ec,
        boost::asio::ip::tcp::resolver::iterator iterator) {
  if (!ec) {
    this->do_read();
  }
}
// }}}
// do_read {{{
void PeerRemote::do_read () {
  async_read (*socket, buffer(msg_inbound), 
      boost::bind(&PeerRemote::on_read, this, ph::error, ph::bytes_transferred));
}
// }}}
// on_read {{{
void PeerRemote::on_read (const boost::system::error_code& ec, size_t s) {
  if (ec) return;
  
  std::string str(msg_inbound.begin(), msg_inbound.end());
  Message* msg = load_message (str);

  if (msg->get_type() == "Boundaries") {
    owner_peer->set_boundaries (msg);
  }
}
// }}}
// do_write {{{
void PeerRemote::do_write (Message* m) {

}
// }}}
// on_read {{{
void PeerRemote::on_write (const boost::system::error_code& ec, size_t s) {

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
