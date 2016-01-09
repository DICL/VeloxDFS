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
    tcp::socket* client_ = new tcp::socket (ioservice);
  
    tcp::resolver::query query (node, to_string(port));
    auto * endpoint_iterator = new tcp::resolver::iterator(
        resolver.resolve(query));

    async_connect (*client_, *endpoint_iterator, bind (
          &MeshTopology::on_connect, this, client_, 
          endpoint_iterator, ph::error, ph::iterator));
    clients_sock.emplace_back(client_);
  }

  acceptor = make_unique<tcp::acceptor> 
    (ioservice, tcp::endpoint(tcp::v4(), port) );

  tcp::socket* server_ =  new tcp::socket(ioservice);
  acceptor->async_accept(*server_, 
      boost::bind (&MeshTopology::on_accept, this,
        server_,
        boost::asio::placeholders::error));
  return true;
}
// }}}
// close {{{
bool MeshTopology::close () {
  return true;
}
// }}}
// on_connect {{{
void MeshTopology::on_connect (tcp::socket* client_,
    tcp::resolver::iterator* it,
    const boost::system::error_code& ec,
    tcp::resolver::iterator it_out) 
{
    tcp::resolver::iterator end;

    cout << "error connect: " << ec.message() << " id " << id;
    cout <<endl;

    if (ec or it_out == end) {
      async_connect (*client_, *it, bind (
            &MeshTopology::on_connect, this, client_, it, 
            ph::error,
            ph::iterator));

    } else {
      delete it;
    }
}
// }}}
// on_accept {{{
void MeshTopology::on_accept (tcp::socket* sock,
    const boost::system::error_code& ec) {
  if (ec) {
    if ( clients_connected < net_size ) {
      acceptor->async_accept(*sock, 
          bind (&MeshTopology::on_accept, this, 
            sock, ph::error));
    }

  } else {
    clients_connected++;
    servers_sock.emplace_back (sock);

    if ( clients_connected < net_size ) {
      tcp::socket* server_ = new tcp::socket(ioservice);
      acceptor->async_accept(*server_,
          bind (&MeshTopology::on_accept, this,
            server_, ph::error));
    } else {
      cout << "Network established with id " << id << endl;
    }
  }
}
// }}}

} /* network */ 
} /* eclipse */ 
