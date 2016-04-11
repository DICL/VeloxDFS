#include "p2p.hh"
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
P2P::P2P(tcp::socket* c, tcp::socket* s, Context& c, AsyncNode* node_) : 
  AsyncChannel(c, node_),
  client(c),
  server(s)
{ }
// }}}
// is_multiple {{{
bool P2P::is_multiple () { return true; }
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
