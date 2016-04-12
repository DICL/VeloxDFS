#include "connector.hh"
#include <string>

using namespace std;
using namespace eclipse::network;
namespace ph = boost::asio::placeholders;

Connector::Connector(int p, NetObserver* o) : 
  nodes (context.settings.get<vec_str>("network.nodes")),
  ip_of_this(context.settings.getip()),
  observer(o), 
  iosvc(context.io),
  port(p) {}

// establish {{{
void Connector::establish () {
  for (auto node : nodes) {
    if (node != ip_of_this)
      do_connect(node);
  }
}
// }}}
// do_connect {{{
void Connector::do_connect (std::string node) {
  tcp::resolver resolver (iosvc);
  tcp::resolver::query query (node, to_string(port));
  tcp::resolver::iterator it (resolver.resolve(query));
  auto ep = new tcp::endpoint (*it);
  auto sock = new tcp::socket(iosvc);

  sock->async_connect (*ep, boost::bind (&Connector::on_connect, this, 
        ph::error, ep, sock));
}
// }}}
// on_connect {{{
void Connector::on_connect (const boost::system::error_code& ec, 
    tcp::endpoint* ep, tcp::socket* sock) {

  if(!ec) {
    delete ep;
    observer->on_connect(sock);

  } else {
    sock->async_connect (*ep, bind (&Connector::on_connect, this, 
        ph::error, ep, sock));
  }
}
