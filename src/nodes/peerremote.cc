#include "peerremote.hh"
#include "../messages/factory.hh"
#include <string>
#include <sstream>
#include <istream>
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
do_read();
}
// }}}
// do_read {{{
void PeerRemote::do_read () {

  async_read_until (*channel->recv_socket(), inbound_data, 
      "\r\r",
      boost::bind(&PeerRemote::on_read, this, ph::error, ph::bytes_transferred));
}
// }}}
// on_read {{{
void PeerRemote::on_read (const boost::system::error_code& ec, size_t s) {

  logger->info ("Message arrived l=%d", s);
  if (!ec)  {

    std::istream buffer(&inbound_data);
     stringstream ss;
     buffer >> ss.rdbuf(); 
    std::string str = ss.str();
    //str << ss; // (msg_inbound.begin(), msg_inbound.end());
    str = str.substr(0, str.find("\r\r"));
    logger->info ("Message arrived %d:%s", str.length(), str.c_str());
    sleep(1);
    Message* msg = load_message (str);
    owner_peer->process_message(msg);
  }

  do_read();
}
// }}}
// do_write {{{
void PeerRemote::do_write (Message* m) {
  string str = save_message (m);
  string tosend = str + "\r\r";

  async_write (*channel->send_socket(), 
      boost::asio::buffer(tosend), boost::bind (
      &PeerRemote::on_write, this, 
      ph::error, ph::bytes_transferred, m));
}
// }}}
// on_write {{{
void PeerRemote::on_write (const boost::system::error_code& ec, 
    size_t s, Message* m) {
  if (ec) {
    logger->info ("Message could not reach err=%s", 
        ec.message().c_str());
    do_write(m);
  } else  {
    logger->info ("Message delivered: %s", 
        ec.message().c_str());
  }
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
