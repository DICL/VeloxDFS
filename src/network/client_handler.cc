#include "client_handler.hh"
#include "../messages/factory.hh"
#include "../common/context_singleton.hh"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/spawn.hpp>

using namespace eclipse::network;
using namespace std;

// Constructor {{{
ClientHandler::ClientHandler (uint32_t p):
  nodes(context.settings.get<vec_str> ("network.nodes")),
  port(p)
{ 
}
// }}}
// connect {{{
void ClientHandler::connect(uint32_t i, shared_ptr<Server> server) {
  spawn(context.io, [&, node=nodes[i], p=this->port](boost::asio::yield_context yield) {
      shared_ptr<Server> s = server;
      boost::system::error_code ec;
      tcp::resolver resolver (context.io);
      tcp::resolver::query query (node, to_string(p));

      auto it = resolver.async_resolve(query, yield[ec]);
      if (ec) {
      ERROR("Resolving %s:%u", s->get_socket().remote_endpoint().address().to_string().c_str(), p);
      return;
      }

      tcp::endpoint ep (*it);
      s->get_socket().async_connect(ep, yield[ec]);
      if (ec) {
      ERROR("Connecting %s:%u", s->get_socket().remote_endpoint().address().to_string().c_str(), p);
      return;
      }

      current_servers.insert({i, s});
      s->do_write_buffer();
      });
}
// }}}
// send {{{
bool ClientHandler::send(uint32_t i, messages::Message* m) {
  if (i >= nodes.size()) return false;

  // If connection is still on.
  if (current_servers.find(i) != current_servers.end()) {
    shared_ptr<Server> ptr = current_servers[i].lock();
    if (ptr) {
      ptr->do_write(m);
      return true;
    }
  } 

  auto server = make_shared<Server>(node);
  shared_ptr<std::string> message_serialized (save_message(m));
  server->commit(message_serialized);
  connect(i, server);

  return true;
}
// }}}
// send str{{{
bool ClientHandler::send(uint32_t i, shared_ptr<string> str) {
  if (i >= nodes.size()) return false;

  // If connection is still on.
  if (current_servers.find(i) != current_servers.end()) {
    shared_ptr<Server> ptr = current_servers[i].lock();
    if (ptr) {
      ptr->do_write(str);
      return true;
    }
  } 

  auto server = make_shared<Server>(node);
  server->commit(str);
  connect(i, server);

  return true;
}
// }}}
// send_and_replicate {{{
bool ClientHandler::send_and_replicate(std::vector<int> node_indices, messages::Message* m) {
  shared_ptr<std::string> message_serialized (save_message(m));
  for (auto i : node_indices) {
    send(i, message_serialized);
  }
  return true;
}
// }}}
