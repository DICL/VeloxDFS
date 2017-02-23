#include "server_handler.hh"
#include "../common/context_singleton.hh"
#include "server.hh"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/spawn.hpp>
#include <memory>
#include <vector>

using namespace eclipse::network;
using namespace std;
using vec_str = std::vector<std::string>;

// Constructor {{{
ServerHandler::ServerHandler (uint32_t p):
  port(p)
{ 
}
// }}}
// establish {{{
bool ServerHandler::establish () {
  auto& iosvc = context.io;

  spawn(iosvc,[&, p=this->port](boost::asio::yield_context yield) {
      DEBUG("Listening at port %u", p);
      tcp::acceptor acceptor (iosvc, tcp::endpoint(tcp::v4(), p) );
        boost::system::error_code ec;
        for (;;) {
          auto server = make_shared<Server>(node);
          acceptor.async_accept(server->get_socket(), yield[ec]);
          DEBUG("Client accepted");
          if (!ec) 
            server->do_read();
        }
      });
  return true;
}
// }}}
// close {{{
bool ServerHandler::close () {
  return true;
}
// }}}
// attach {{{
void ServerHandler::attach (NetObserver* node_) {
  node = node_;
}
// }}}
