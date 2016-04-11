#include "p2p.hh"
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
P2P::P2P(Context& c, int i, AsyncNode* node_) : 
  AsyncChannel(c, i, node_),
  client(c.io)
{ }
// }}}
// do_connect {{{
void P2P::do_connect () {
  tcp::resolver resolver (iosvc);
  tcp::resolver::query query (host, to_string(port));
  tcp::resolver::iterator it (resolver.resolve(query));
  endpoint = new tcp::endpoint (*it);

  client.async_connect (*endpoint, bind (&P2P::on_connect, this, 
        ph::error));
}
// }}}
// on_connect {{{
void P2P::on_connect (const boost::system::error_code& ec) {
  if(!ec) {
    client_connected = true;
    if(client_connected and server_connected) {
      do_read();
    }
  } else {

  client.async_connect (*endpoint, bind (&P2P::on_connect, this, 
        ph::error));
  }
}
// }}}
// on_accept {{{
void P2P::on_accept (tcp::socket* sock) {
  server = sock;
  server_connected = true;
  if(client_connected and server_connected) {
    do_read();
  }
}
// }}}
// do_read {{{
void P2P::do_read () {
  logger->info("Connection established, starting to read");
  spawn(iosvc, bind(&P2P::read_coroutine, this, _1));
}
// }}}
// do_write {{{
void P2P::do_write (Message* m) {
  string str = save_message (m);
  stringstream ss; 
  ss << setfill('0') << setw(header_size) << str.length() << str;
  string* to_write = new string(ss.str());

  async_write (client, buffer(*to_write), boost::bind (&P2P::on_write, this, 
      ph::error, ph::bytes_transferred, m, to_write));
}
// }}}
// on_write {{{
void P2P::on_write (const boost::system::error_code& ec, 
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
void P2P::read_coroutine (yield_context yield) {
  char * body;
  boost::system::error_code ec;
  char header [17]; 
  auto* sock = server;

  while (true) {
    bzero(header, 16);
    header[16] = '\0';
    size_t l = async_read (*sock, buffer(header, 16), yield[ec]);
    if (l != (size_t)header_size) continue;

    size_t size = atoi(header);
    body = new char[size];
    l = async_read (*sock, buffer(body, size), yield[ec]);

    if (!ec)  {
      string str (body);
      delete[] body;

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
