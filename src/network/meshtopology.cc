#include "meshtopology.hh"
#include <boost/bind.hpp>
#include <iostream>

namespace eclipse {
namespace network {

using namespace std;
using boost::bind;
namespace ph = boost::asio::placeholders;

// establish {{{
bool MeshTopology::establish () {
  net_size = static_cast<int> (nodes.size());
  
  tcp::resolver resolver (ioservice);
  string myself = nodes[id];

  for (auto node : nodes) {
    if (node == myself) continue;

    tcp::resolver::query query (node, to_string(port));
    tcp::resolver::iterator it (resolver.resolve(query));

    auto endpoint = new tcp::endpoint (*it);
    auto client = new tcp::socket (ioservice);

    client->async_connect (*endpoint, bind (
          &MeshTopology::on_connect, this, ph::error, client, 
          endpoint));

    clients_sock.emplace_back(client);
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
  return true;
}
// }}}
// on_connect {{{
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
      delete it;
    }
}
// }}}
// on_accept {{{
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

  } else {
    clients_connected++;
    servers_sock.emplace_back (sock);

    if ( clients_connected < net_size ) {
      auto server = new tcp::socket(ioservice);
      acceptor->async_accept(*server,
          bind (&MeshTopology::on_accept, this,
            ph::error, server));
    } else {
      cout << "Network established with id " << id << endl;
    }
  }
}
// }}}

} /* network */ 
} /* eclipse */ 
