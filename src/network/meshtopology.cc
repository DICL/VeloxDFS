#include "meshtopology.hh"
#include <boost/bind.hpp>
#include <iostream>

namespace eclipse {
namespace network {

using namespace std;
using boost::bind;
namespace ph = boost::asio::placeholders;

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
      auto client = new tcp::socket (ioservice);

      client->async_connect (*endpoint, bind (
            &MeshTopology::on_connect, this, ph::error, client, 
            endpoint));

      auto channel = new Channel(client);
      channels.insert (make_pair(i, channel));
    }
    i++;
  }

  acceptor = make_unique<tcp::acceptor> 
    (ioservice, tcp::endpoint(tcp::v4(), port) );

  auto server = new tcp::socket(ioservice);
  acceptor->async_accept (*server, 
      bind (&MeshTopology::on_accept, this, ph::error, server));

  return true;
}
// }}}
// close {{{
bool MeshTopology::close () {
  for (auto p: channels) {
    auto channel = p.second;

    channel->send_socket()->close();
    channel->recv_socket()->close();
  }

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
// on_accept {{{
// @brief handle a connection with a client
// @todo refactor
void MeshTopology::on_accept (
    const boost::system::error_code& ec,
    tcp::socket* sock) 
{
  if (ec) {
    if ( clients_connected < net_size ) {
      acceptor->async_accept(*sock, 
          bind (&MeshTopology::on_accept, this, 
            ph::error, sock));
    }
    sleep (1);

      acceptor->async_accept(*sock, 
          bind (&MeshTopology::on_accept, this, 
            ph::error, sock));

  } else {
    auto ep = sock->remote_endpoint();
    auto address = ep.address().to_string();

    int index = 0;
    for (auto node : nodes) {
      if (node == address) break;
      index++;
    }

    channels[index]->set_recv_socket(sock);
    clients_connected++;
    logger->info ("Accepted client id=%d", index);
    if ( clients_connected < net_size ) {
      auto server = new tcp::socket(ioservice);
      acceptor->async_accept(*server,
          bind (&MeshTopology::on_accept, this,
            ph::error, server));
    } else {
      logger->info ("Network established with id=%d",id);
      auto server = new tcp::socket(ioservice);
      acceptor->async_accept(*server,
          bind (&MeshTopology::on_accept, this,
            ph::error, server)); sleep (1);
    }
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
