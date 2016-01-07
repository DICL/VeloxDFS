#include "ringtopology.hh"
#include <boost/bind.hpp>
#include <iostream>

namespace eclipse {
namespace network {

using namespace std;
using boost::bind;
namespace ph = boost::asio::placeholders;

// establish {{{
bool RingTopology::establish () {
  string to_accept;
  string to_connect;

  tie(to_accept, to_connect) = organize();

  client_sock = make_unique<tcp::socket> (ioservice); 
  server_sock = make_unique<tcp::socket> (ioservice);
  client = make_unique<Channel> (*client_sock);
  client = make_unique<Channel> (*server_sock);

  tcp::resolver resolver (ioservice);
  tcp::resolver::query query (to_connect, to_string(port));
  endpoint_iterator = make_unique<tcp::resolver::iterator> (
    resolver.resolve(query));

  async_connect (*client_sock, *endpoint_iterator, bind (
      &RingTopology::on_connect, this, ph::error,
      ph::iterator));

  acceptor = make_unique<tcp::acceptor> 
    (ioservice, tcp::endpoint(tcp::v4(), port) );

  acceptor->async_accept(*server_sock, 
      boost::bind (&RingTopology::on_accept, this,
        boost::asio::placeholders::error));

  return true;
}
// }}}
// close {{{
bool RingTopology::close () {
  try {
    client_sock->close();
    server_sock->close();
  } catch (...) {
  
  }
}
// }}}
// on_connect {{{
void RingTopology::on_connect (
    const boost::system::error_code& ec, 
    boost::asio::ip::tcp::resolver::iterator it) {

  if (ec) {
    async_connect (*client_sock, *endpoint_iterator, bind (
        &RingTopology::on_connect, this, ph::error,
        ph::iterator));
    // An error occurred.
  } else {
    cout << "Network established" << endl;
  }
}
// }}}
// on_accept {{{
void RingTopology::on_accept (
    const boost::system::error_code& ec) {

}
// }}}
// organize {{{
std::tuple<string,string> RingTopology::organize () {
  string to_accept;
  string to_connect;
  size_t size = nodes.size() - 1;

  if (id != 0 and id != size) {
    to_accept = nodes[id-1];
    to_connect = nodes[id+1];

  } else if (id == 0) {
    to_accept = nodes[size];
    to_connect = nodes[id+1];
  
  } else if (id == size) {
    to_accept = nodes[id-1];
    to_connect = nodes[0];
  }

  return make_tuple(to_accept, to_connect);
}
// }}}

} /* network */ 
} /* eclipse */ 
