#include "meshtopology.hh"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <iostream>

namespace eclipse {
namespace network {

using namespace std;
using boost::bind;
namespace ph = boost::asio::placeholders;

// Constructor {{{
MeshTopology::MeshTopology(Context& c, 
    std::map<int, Channel*>* _map, AsyncNode* node_) :
  Topology (c), node (node_), channels(_map) {}
// }}}
// establish {{{
// @brief It establish the network, 
// connecting with other nodes
// @todo  refactor
bool MeshTopology::establish () {
  net_size = static_cast<int> (nodes.size());
  
  tcp::resolver resolver (ioservice);
  string myself = nodes[id];

  int i = 0;
  for (auto node : nodes) {
    if (node != myself) {

      tcp::resolver::query query (node, to_string(port));
      tcp::resolver::iterator it (resolver.resolve(query));

      auto endpoint = new tcp::endpoint (*it);
      auto client = (*channels)[i]->send_socket;

      client->async_connect (*endpoint, bind (
            &MeshTopology::on_connect, this, ph::error, client, 
            endpoint));
    }
    i++;
  }

  acceptor = make_unique<tcp::acceptor> 
    (ioservice, tcp::endpoint(tcp::v4(), port) );

  spawn(ioservice, bind(&MeshTopology::accept, this, _1));

  return true;
}
// }}}
// close {{{
bool MeshTopology::close () {
  return true;
}
// }}}
// on_connect {{{
// @brief handle a connection to a server
void MeshTopology::on_connect (
    const boost::system::error_code& ec,
    tcp::socket* client,
    tcp::endpoint* it) 
{

    if (ec) {
      client->async_connect (*it, bind (
            &MeshTopology::on_connect, this, ph::error, 
            client, it));

    } else {
      server_connected++;
      delete it;
    }
}
// }}}
// accept {{{
// @brief handle a connection with a client
// @todo refactor
void MeshTopology::accept (boost::asio::yield_context yield)
{ 
  tcp::socket* server = nullptr;

  try {
    while (clients_connected < net_size) {
      server = new tcp::socket(ioservice);
      acceptor->async_accept (*server, yield);

      auto ep = server->remote_endpoint();
      auto address = ep.address().to_string();

      int index = 0;
      for (auto node : nodes) {
        if (node == address) break;
        index++;
      }

      clients_connected++;
      (*channels)[index]->update_recv(server);
      logger->info ("Accepted client id=%d", index);
    }
    node->on_connect();

  } catch (std::exception& e) {
      acceptor->async_accept (*server, yield);
  }
}
// }}}
// is_online {{{
bool MeshTopology::is_online () {
  return (net_size == clients_connected);
}
// }}}

} /* network */ 
} /* eclipse */ 
