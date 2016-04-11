#include "connector.hh"

Connector::Connector(Context& c, int p, NetObserver* o) : 
  nodes (c.settings.get<vec_str>("network.nodes")),
  observer(o), 
  iosvc(c.io),
  port(p) {}

// establish {{{
void Connector::establish () {
  for (auto node : nodes) do_connect(node);
}
// }}}
// do_connect {{{
void Connector::do_connect (std::string node) {
  tcp::resolver resolver (iosvc);
  tcp::resolver::query query (node, to_string(port));
  tcp::resolver::iterator it (resolver.resolve(query));
  ep = new tcp::endpoint (*it);
  auto sock = tcp::socket(iosvc);

  sock->async_connect (*ep, bind (&Connector::on_connect, this, 
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
