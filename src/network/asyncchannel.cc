#include "asyncchannel.hh"
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
namespace network {
// constructor {{{
AsyncChannel::AsyncChannel(Context& c, int i, AsyncNode* node_) : 
  Channel(c, i),
  node (node_)
{ }
// }}}
// on_connect {{{
void AsyncChannel::on_connect () {
  do_read();
}
// }}}
// do_read {{{
void AsyncChannel::do_read () {
  spawn(iosvc, bind(&AsyncChannel::read_coroutine, this, _1));
}
// }}}
// do_write {{{
void AsyncChannel::do_write (Message* m) {
  string str = save_message (m);
  stringstream ss; 
  ss << setfill('0') << setw(header_size) << str.length() << str;

  async_write (*send_socket, 
      buffer(ss.str()), boost::bind (&AsyncChannel::on_write, this, 
      ph::error, ph::bytes_transferred, m));
}
// }}}
// on_write {{{
void AsyncChannel::on_write (const boost::system::error_code& ec, 
    size_t s, Message* m) {
  if (ec) {
    logger->info ("Message could not reach err=%s", 
        ec.message().c_str());

    do_write(m);

  } 
}
// }}}
// read_coroutine {{{
void AsyncChannel::read_coroutine (yield_context yield) {
  boost::asio::streambuf body; 
  boost::system::error_code ec;
  char header [16]; 
  auto* sock = recv_socket;

  while (true) {
    size_t l = async_read (*sock, buffer(header), yield[ec]);
    if (l != (size_t)header_size) continue;

    size_t size = atoi(header);
    l = async_read (*sock, body.prepare(size), yield[ec]);

    if (!ec)  {
      body.commit (l);
      string str ((istreambuf_iterator<char>(&body)), 
          istreambuf_iterator<char>());
      body.consume (l);

      Message* msg = nullptr;
      msg = load_message(str);
      node->on_read(msg);
      delete msg;

    } else {
      logger->info ("Message arrived error=%s", 
          ec.message().c_str());
    }
  }
}
// }}}
}
} /* eclipse */
