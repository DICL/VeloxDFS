#include "server.hh"
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
Server::Server(Context& c, int i, AsyncNode* node_) : 
  AsyncChannel(c, i, node_)
{ }
// }}}
// do_connect {{{
void Server::do_connect () {
  //The result of a bad design leads to this empty function :D
}
// }}}
// on_accept {{{
void Server::on_accept (tcp::socket* sock) {
  if (server != nullptr) delete server;
  server = sock;
  do_read();
}
// }}}
// do_read {{{
void Server::do_read () {
  logger->info("Connection established, starting to read");
  spawn(iosvc, bind(&Server::read_coroutine, this, _1));
}
// }}}
// do_write {{{
void Server::do_write (Message* m) {
  string str = save_message (m);
  stringstream ss; 
  ss << setfill('0') << setw(header_size) << str.length() << str;

  async_write (*server, 
      buffer(ss.str()), boost::bind (&Server::on_write, this, 
      ph::error, ph::bytes_transferred, m));
}
// }}}
// on_write {{{
void Server::on_write (const boost::system::error_code& ec, 
    size_t s, Message* m) {
  if (ec) {
    logger->info ("Message could not reach err=%s", 
        ec.message().c_str());

    do_write(m);

  } 
}
// }}}
// read_coroutine {{{
void Server::read_coroutine (yield_context yield) {
  boost::asio::streambuf body; 
  boost::system::error_code ec;
  char header [17]; 
  header[16] = '\0';
  auto* sock = server;

  try {
    while (true) {
      size_t l = async_read (*sock, buffer(header, 16), yield[ec]);
      if (ec) throw 1;
      if (l != (size_t)header_size) continue;

      size_t size = atoi(header);
      l = async_read (*sock, body.prepare(size), yield[ec]);
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
    if (ec == boost::asio::error::eof) {
      logger->info ("Closing server socket to client");
      server->close();
      delete server;
      server = nullptr;
      node->on_disconnect();
    } else {
      logger->info ("Message arrived error=%s", 
          ec.message().c_str());
    }
  }
}
// }}}
}
} /* eclipse */
