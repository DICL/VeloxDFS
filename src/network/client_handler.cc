#include "client_handler.hh"
#include "../messages/factory.hh"
#include "../common/context_singleton.hh"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/spawn.hpp>
#include <exception>
#include <boost/scoped_ptr.hpp>

using namespace eclipse::network;
using namespace std;
using boost::scoped_ptr;
using boost::asio::ip::tcp;
using vec_str = std::vector<std::string>;

// Constructor {{{
ClientHandler::ClientHandler (uint32_t p):
  nodes(context.settings.get<vec_str> ("network.nodes")),
  port(p),
  id(context.id)
{ 
}
void ClientHandler::attach(NetObserver* n) {
  local_router = n;
}
// }}}
// connect {{{
void ClientHandler::connect(uint32_t i, shared_ptr<Server> server) {
  spawn(context.io, [this, index = i, server_copy=server, node=nodes[i]]
      (boost::asio::yield_context yield) {
        try {
          shared_ptr<Server> s = server_copy;
          boost::system::error_code ec;
          tcp::resolver resolver (context.io);
          tcp::resolver::query query (node, to_string(port));

          auto it = resolver.async_resolve(query, yield[ec]);
          if (ec) 
            BOOST_THROW_EXCEPTION(std::runtime_error("Resolving"));

          tcp::endpoint ep (*it);

          s->get_socket().async_connect(ep, yield[ec]);
          while (ec == boost::asio::error::timed_out) {
            s->get_socket().close();
            WARN("Re-connecting to %s:%u", node.c_str(), port);
            s->get_socket().async_connect(ep, yield[ec]);
          } 

          if (ec) 
            BOOST_THROW_EXCEPTION(std::runtime_error("Connecting"));

          tcp::no_delay option(true);
          s->get_socket().set_option(option); 

          rw_lock.lock();
          current_servers.insert({index, s});
          rw_lock.unlock();

          s->do_write_buffer();

        } catch (exception& e) {
          ERROR("Connect coroutine exception %s", e.what());
          throw;

        } catch (boost::exception& e) {
          ERROR("Connect corourine exception %s", diagnostic_information(e).c_str());
          throw;
        }
      });
}
// }}}
// try_reuse_client {{{
bool ClientHandler::try_reuse_client(uint32_t i, shared_ptr<std::string> str) {

  // If connection is still on.
  rw_lock.lock_shared();
  auto it = current_servers.find(i); 
  rw_lock.unlock_shared();

  if (it != current_servers.end()) {
    shared_ptr<Server> ptr = current_servers[i].lock();
    if (ptr) {
      DEBUG("REUSING SOCKET");
      ptr->do_write(str);
      return true;

    } else {
      rw_lock.lock();
      current_servers.erase(i);
      rw_lock.unlock();
    }
  } 
  return false;
}
// }}}
// send {{{
bool ClientHandler::send(uint32_t i, messages::Message* m) {
  if (i >= nodes.size()) return false;

  shared_ptr<std::string> message_serialized (save_message(m));

  if (!try_reuse_client(i, message_serialized)) {
    auto server = make_shared<Server>(local_router);
    server->commit(message_serialized);
    connect(i, server);
  }

  return true;
}
// }}}
// send str{{{
bool ClientHandler::send(uint32_t i, shared_ptr<string> str) {
  if (i >= nodes.size()) return false;

  if (!try_reuse_client(i, str)) {
    auto server = make_shared<Server>(local_router);
    server->commit(str);
    connect(i, server);
  }

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
