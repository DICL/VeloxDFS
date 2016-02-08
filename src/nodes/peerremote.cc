#include "peerremote.hh"
#include "../messages/factory.hh"
#include <string>
#include <sstream>
#include <iomanip>
#include <istream>
#include <boost/bind.hpp>

using namespace std;
using namespace boost::asio;
namespace ph = boost::asio::placeholders;

namespace eclipse {
// constructor {{{
PeerRemote::PeerRemote(NodeLocal* p, int i) : NodeRemote(p, i) { }
// }}}
// action {{{
void PeerRemote::action () {
  do_read();
}
// }}}
// do_read {{{
void PeerRemote::do_read () {
  spawn(ioservice, bind(&PeerRemote::read_coroutine, this, _1));
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
// read_coroutine {{{
void PeerRemote::read_coroutine (yield_context yield) {
  boost::asio::streambuf body; 
  boost::system::error_code ec;
  char header [16]; 
  auto* sock = channel->recv_socket();

  while (true) {
    size_t l = async_read (*sock, buffer(header), yield[ec]);
    if (l != (size_t)header_size) continue;

    size_t size = atoi(header);
    logger->info ("Header received size=%d:%d", l, size);

    l = async_read (*sock, body.prepare(size), yield[ec]);

    if (!ec)  {
      logger->info ("Message arrived size=%d");

      body.commit (l);
      string str ((istreambuf_iterator<char>(&body)), 
          istreambuf_iterator<char>());
      body.consume (l);

      Message* msg = nullptr;
      msg = load_message(str);
      owner->process_message(msg);
      delete msg;

    } else {
      logger->info ("Message arrived error=%s", 
          ec.message().c_str());
    }
  }
}
// }}}
} /* eclipse */
