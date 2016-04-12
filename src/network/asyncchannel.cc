#include "asyncchannel.hh"
#include "../messages/factory.hh"
#include <string>
#include <sstream>
#include <iomanip>
#include <istream>
#include <boost/bind.hpp>

namespace ph = boost::asio::placeholders;
using namespace std;
using namespace boost::asio;
using namespace eclipse::network;
using namespace eclipse::messages;
using namespace boost::system;

// constructor {{{
AsyncChannel::AsyncChannel(tcp::socket* s, tcp::socket* r, AsyncNode* node_) : 
  node (node_),
  sender(s),
  receiver(r)
{ }
// }}}
// do_write {{{
void AsyncChannel::do_write (Message* m) {
  string str = save_message (m);
  stringstream ss; 
  ss << setfill('0') << setw(header_size) << str.length() << str;

  string* to_write = new string(ss.str());

  async_write (*sender, buffer(*to_write), boost::bind (&AsyncChannel::on_write, this, 
      ph::error, ph::bytes_transferred, m, to_write));
}
// }}}
// on_write {{{
void AsyncChannel::on_write (const boost::system::error_code& ec, 
    size_t s, Message* m, string* str) {
  delete str;
  if (ec) {
    logger->info ("Message could not reach err=%s", 
        ec.message().c_str());

    do_write(m);

  } 
}
// }}}
// do_read {{{
void AsyncChannel::do_read () {
  logger->info("Connection established, starting to read");
  spawn(iosvc, bind(&AsyncChannel::read_coroutine, this, _1));
}
// }}}
// read_coroutine {{{
void AsyncChannel::read_coroutine (yield_context yield) {
  boost::asio::streambuf body; 
  boost::system::error_code ec;
  char header [header_size + 1]; 
  header[16] = '\0';

  try {
    while (true) {
      size_t l = async_read (*receiver, buffer(header, header_size), yield[ec]);
      if (ec) throw 1;
      if (l != (size_t)header_size) continue;

      size_t size = atoi(header);
      l = async_read (*receiver, body.prepare(size), yield[ec]);
      if (ec) throw 1;

      body.commit (l);
      string str ((istreambuf_iterator<char>(&body)), 
          istreambuf_iterator<char>());
      body.consume (l);

      Message* msg = nullptr;
      msg = load_message(str);
      node->on_read(msg);
      delete msg;
    }
  } catch (...) {
    if (ec == boost::asio::error::eof)
      logger->info ("AsyncChannel: Closing server socket to client");
    else
      logger->info ("AsyncChannel: Message arrived error=%s", 
          ec.message().c_str());

      if (receiver!= nullptr) 
        receiver->close();

      delete receiver;
      receiver = nullptr;
      node->on_disconnect();
  }
}
// }}}
