#include "ringtopology.hh"
#include <boost/bind.hpp>

namespace eclipse {
namespace network {

using namespace std;

// establish {{{
bool RingTopology::establish () {
  string to_accept;
  string to_connect;

  tie(to_accept, to_connect) = organize();

  client_sock = make_unique<tcp::socket> (ioservice);
  server_sock = make_unique<tcp::socket> (ioservice);

  tcp::resolver resolver(ioservice);
  tcp::resolver::query query(to_connect, to_string(port));
  tcp::resolver::iterator endpoint_iterator = 
    resolver.resolve(query);

  async_connect (*client_sock, endpoint_iterator, 
      boost::bind (&RingTopology::on_connect, this, 
        boost::asio::placeholders::error,
        boost::asio::placeholders::iterator));

  tcp::acceptor acceptor
    (ioservice, tcp::endpoint(tcp::v4(), port) );

  acceptor.async_accept(*server_sock, 
      boost::bind (&RingTopology::on_accept, this,
        boost::asio::placeholders::error));

  return true;
}
// }}}
// close {{{
bool RingTopology::close () {
  client_sock->close();
  server_sock->close();
}
// }}}
// on_connect {{{
void RingTopology::on_connect (
    const boost::system::error_code& ec, 
    boost::asio::ip::tcp::resolver::iterator it) {
}
// }}}
// on_accept {{{
void RingTopology::on_accept (
    const boost::system::error_code&) {
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
