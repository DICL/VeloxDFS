#include "server.hh"
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

// constructor {{{
Server::Server(tcp::socket* s, Context& c, AsyncNode* node_) : 
  AsyncChannel(c, i, node_),
  server(s)
{ }
// }}}
// is_multiple {{{
bool P2P::is_multiple () { return false; }
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

  string* to_write = new string(ss.str());

  async_write (*server, buffer(*to_write), boost::bind (&Server::on_write, this, 
      ph::error, ph::bytes_transferred, m, to_write));
}
// }}}
// on_write {{{
void Server::on_write (const boost::system::error_code& ec, 
    size_t s, Message* m, string* str) {
  delete str;
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
      logger->info ("Server: l=%d", l);
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
      logger->info ("Server: Closing server socket to client");
    else
      logger->info ("Server: Message arrived error=%s", 
          ec.message().c_str());

      if (server != nullptr) 
        server->close();

      delete server;
      server = nullptr;
      node->on_disconnect();
  }
}
// }}}
