#pragma once

#include "netobserver.hh"
#include "../common/context_singleton.hh"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/spawn.hpp>
#include <memory>

namespace eclipse {
namespace network {

using boost::asio::ip::tcp;
using vec_str = std::vector<std::string>;

class Acceptor {
  public:
    Acceptor(int, NetObserver*);
    void listen();

  protected:
    void do_listen(boost::asio::yield_context );

    std::unique_ptr<tcp::acceptor> acceptor;
    NetObserver* observer = nullptr;
    boost::asio::io_service& iosvc;
    vec_str nodes;
    int port = 0;
};

} /* network  */ 
}
