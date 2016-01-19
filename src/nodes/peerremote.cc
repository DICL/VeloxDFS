#include "peerremote.hh"
#include "../messages/factory.hh"
#include <string>
#include <boost/bind.hpp>

using namespace std;
using namespace std::placeholders;
namespace ph = boost::asio::placeholders;

namespace eclipse {
class PeerLocal;
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
// start {{{
void PeerRemote::start () {
}
// }}}
// do_read {{{
void PeerRemote::do_read () {
  async_read (*channel->get_socket(), buffer(msg_inbound), 
      boost::bind(&PeerRemote::on_read, this, ph::error, ph::bytes_transferred));
}
// }}}
// on_read {{{
void PeerRemote::on_read (const boost::system::error_code& ec, size_t s) {
  if (!ec)  {

    logger->info ("Message arrived");
    std::string str(msg_inbound.begin(), msg_inbound.end());
    Message* msg = load_message (str);
    owner_peer->process_message(msg);
  }

  do_read();
}
// }}}
// do_write {{{
void PeerRemote::do_write (Message* m) {
  string str = save_message (m);
  string *tosend = new string(str);

  async_write (*channel->get_socket(), 
      boost::asio::buffer(*tosend), boost::bind (
      &PeerRemote::on_write, this, 
      ph::error, ph::bytes_transferred));
}
// }}}
// on_write {{{
void PeerRemote::on_write (const boost::system::error_code& ec, size_t s) {

}
// }}}
// send {{{
void PeerRemote::send(Message* m) {
  string tosend;
  tosend << m;
 // socket->send (boost::asio::buffer(tosend));
}
// }}}
} /* nodes  */
