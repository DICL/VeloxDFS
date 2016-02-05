#include "peerremote.hh"
#include "../messages/factory.hh"
#include <string>
#include <sstream>
#include <iomanip>
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
// action {{{
void PeerRemote::action () {
  do_read();
}
// }}}
// do_read {{{
void PeerRemote::do_read () {

  async_read (*channel->recv_socket(), buffer(inbound_header), 
      transfer_exactly(header_size),
      boost::bind(&PeerRemote::on_read_header, this, 
        ph::error, ph::bytes_transferred));
}
// }}}
// on_read_header {{{
void PeerRemote::on_read_header (const boost::system::error_code& ec, size_t s) {

  if (!ec)  {
    size_t size = atoi(inbound_header);
    logger->info ("Header received size=%d:%d", s, size);

    if(size == 0) {
      logger->info ("Header received : %s", inbound_header);
    }
    async_read (*channel->recv_socket(),
        inbound_data.prepare(size),
       boost::bind(
        &PeerRemote::on_read_body, this, 
        ph::error, ph::bytes_transferred));
  } else {
    do_read();
  }
}
// }}}
// on_read_body {{{
void PeerRemote::on_read_body (const boost::system::error_code& ec,
    size_t s) {

  if (!ec)  {
    logger->info ("Message arrived size=%d");

    inbound_data.commit(s);

    std::string str((istreambuf_iterator<char>(&inbound_data)), 
                     istreambuf_iterator<char>());
    inbound_data.consume(s);

    Message* msg = nullptr;
    msg = load_message(str);
    owner_peer->process_message(msg);
    delete msg;

  } else {
    logger->info ("Message arrived error=%s", ec.message().c_str());
  }

  do_read();
}
// }}}
// do_write {{{
void PeerRemote::do_write (Message* m) {
  string str = save_message (m);
  stringstream ss; 
  ss << setfill('0') << setw(header_size) << str.length() << str;

  async_write (*channel->send_socket(), 
      buffer(ss.str()), boost::bind (&PeerRemote::on_write, this, 
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
    logger->info ("Message delivered: %s", ec.message().c_str());
  }
}
// }}}
} /* eclipse */
