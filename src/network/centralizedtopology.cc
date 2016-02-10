#include "centralizedtopology.hh"
#include <boost/bind.hpp>
#include <iostream>

namespace eclipse {
namespace network {

using namespace std;
using boost::bind;
namespace ph = boost::asio::placeholders;

// constructor {{{
CentralizedTopology::CentralizedTopology(Context& c, 
    Channel* channel_, AsyncNode* node_): 
  Topology(c), channel(channel_), node(node_) 
{

}
//}}}
// establish {{{
bool CentralizedTopology::establish () {
  acceptor = make_unique<tcp::acceptor> 
    (ioservice, tcp::endpoint(tcp::v4(), port) );

  auto server = channel->send_socket;
  channel->update_recv (server);
  acceptor->async_accept (*server, 
      bind (&CentralizedTopology::on_accept, this, 
        ph::error, server));
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
//void CentralizedTopology::on_connect (
//    const boost::system::error_code& ec,
//    tcp::socket* client,
//    tcp::endpoint* it) 
//{
//
//  if (ec) {
//    client->async_connect (*it, bind (
//          &CentralizedTopology::on_connect, this, ph::error, 
//          client, it));
//
//  } else {
//    logger->info ("connection established id=%d", id);
//    owner->action (client);
//  }
//}
// }}}
// on_accept {{{
// @brief handle a connection with a client
// @todo refactor
void CentralizedTopology::on_accept (
    const boost::system::error_code& ec)  
{
  if (!ec) {
      auto ep = sock->remote_endpoint();
      auto address = ep.address().to_string().c_str();

      logger->info ("Accepted client id=%s", address);
      node->on_connect();
      channel->on_connect();
      return;
    }

  } else {
    acceptor->async_accept(*(channel->send_socket),
        bind (&CentralizedTopology::on_accept, this,
          ph::error)); 
  }

}
// }}}

} /* network  */ 
} /* eclipse  */ 
