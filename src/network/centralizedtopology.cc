#include "centralizedtopology.hh"
#include <boost/bind.hpp>
#include <iostream>

namespace eclipse {
namespace network {

using namespace std;
using boost::bind;
namespace ph = boost::asio::placeholders;

// constructor {{{
CentralizedTopology::CentralizedTopology(
    boost::asio::io_service& io, Logger* l, 
    std::string master, vec_str net, int port , int id) : 
  Topology(io, l , net, port, id), master(master){
    net_size = static_cast<int> (net.size());

    if (master == net[id]) {
      type = "master";
    
    } else {
      type = "slave";
    }
}
//}}}
// establish {{{
bool CentralizedTopology::establish () {
  if (type == "master") {
    acceptor = make_unique<tcp::acceptor> 
      (ioservice, tcp::endpoint(tcp::v4(), port) );

    auto server = new tcp::socket(ioservice);
    acceptor->async_accept (*server, 
        bind (&CentralizedTopology::on_accept, this, 
          ph::error, server));

  } else {
    tcp::resolver resolver (ioservice);
    tcp::resolver::query query (master, to_string(port));
    tcp::resolver::iterator it (resolver.resolve(query));

    auto endpoint = new tcp::endpoint (*it);
    auto client = new tcp::socket (ioservice);

    client->async_connect (*endpoint, bind (
          &CentralizedTopology::on_connect, this, 
          ph::error, client, endpoint));

    auto channel = new Channel(client);
    channels.insert (make_pair(0, channel));
  }
  return true;
}
// }}}
// close {{{
bool CentralizedTopology::close () {
  return true;
}
// }}}
// is_online {{{
bool CentralizedTopology::is_online () {
  return true;
}
// }}}
// on_connect {{{
// @brief handle a connection to a server
void CentralizedTopology::on_connect (
    const boost::system::error_code& ec,
    tcp::socket* client,
    tcp::endpoint* it) 
{

  if (ec) {
    client->async_connect (*it, bind (
          &CentralizedTopology::on_connect, this, ph::error, 
          client, it));

  } else {
    logger->info ("connection established id=%d", id);

    string str;
    static boost::asio::streambuf sbuf;
    async_read(*client, sbuf, bind(
          &CentralizedTopology::dummy_callback, this, 
          ph::error));
  }
}
// }}}
// on_accept {{{
// @brief handle a connection with a client
// @todo refactor
void CentralizedTopology::on_accept (
    const boost::system::error_code& ec,
    tcp::socket* sock)  
{
  tcp::socket* next_socket = nullptr;

  if (!ec) {
    next_socket = new tcp::socket(ioservice);
    if (clients_connected < net_size) {
      auto ep = sock->remote_endpoint();
      auto address = ep.address().to_string();

      int index = find (nodes.begin(), nodes.end(), address) - nodes.begin() ;

      auto channel = new Channel(sock);
      channels.insert (make_pair(index, channel));
      clients_connected++;
      logger->info ("Accepted client id=%d", index);

    } else { 
      logger->info ("Network established with id=%d",id);
    }

  } else {
    next_socket = sock;
  }

  acceptor->async_accept(*next_socket,
      bind (&CentralizedTopology::on_accept, this,
        ph::error, next_socket)); 

  sleep (1);
}
// }}}
// dummy_callback {{{
void CentralizedTopology::dummy_callback (const boost::system::error_code&) {
}
// }}}

} /* network  */ 
} /* eclipse  */ 
