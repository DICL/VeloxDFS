#include "acceptor.hh"

using namespace eclipse::network;

// Constructor {{{
Acceptor::Acceptor(int port_, NetObserver* o):
  observer(o),
  iosvc (context.io),
  nodes (context.settings.get<vec_str>("network.nodes")),
  port  (port_)
{ }
// }}}
// listen {{{
void Acceptor::listen () {
  acceptor = std::make_unique<tcp::acceptor> (iosvc,
      tcp::endpoint(tcp::v4(), port) );

  spawn(iosvc, bind(&Acceptor::do_listen, this, _1));
}
// }}}
// do_listen {{{
void Acceptor::do_listen (boost::asio::yield_context yield) {
  boost::system::error_code ec;
  for (;;) {
    auto socket = new tcp::socket(iosvc);
    acceptor->async_accept (*socket, yield[ec]);
    if (!ec) 
      observer->on_accept(socket);
  }
}
// }}}
