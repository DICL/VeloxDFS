#pragma once

#include "../common/context.hh"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/spawn.hpp>
#include <memory>

namespace eclipse {
namespace network {

using boost::asio::ip::tcp;
using vec_str = std::vector<std::string>;

template <typename T>
class Acceptor {
  public:
    Acceptor(Context&, T*);

    void listen();

  protected:
    void do_listen(boost::asio::yield_context );

    std::unique_ptr<tcp::acceptor> acceptor;
    T* callback;
    boost::asio::io_service& iosvc;
    vec_str nodes;
    int port;
};

// Constructor {{{
template <typename T>
Acceptor<T>::Acceptor(Context& c, T* fun):
  callback(fun),
  iosvc (c.io),
  nodes (c.settings.get<vec_str>("network.nodes")),
  port  (c.settings.get<int>("network.port_cache"))
{ }
// }}}
// listen {{{
template <typename T>
void Acceptor<T>::listen () {
  acceptor = std::make_unique<tcp::acceptor> (iosvc,
      tcp::endpoint(tcp::v4(), port) );

  spawn(iosvc, bind(&Acceptor<T>::do_listen, this, _1));
}
// }}}
// do_listen {{{
template <typename T>
void Acceptor<T>::do_listen (boost::asio::yield_context yield) {
  for (;;) {
    auto socket = new tcp::socket(iosvc);
    acceptor->async_accept (*socket, yield);

    auto ep = socket->remote_endpoint();
    auto address = ep.address().to_string();

    int index = 0;
    for (auto node : nodes) {
      if (node == address) break;
      index++;
    }

    callback->on_accept(index, socket);
  }
}
// }}}
} /* network  */ 
}
