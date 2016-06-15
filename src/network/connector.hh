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

using vec_str = std::vector<std::string>;

class Connector {
  public:
    Connector(int, NetObserver*); 
    ~Connector() = default; 

    void establish ();

  protected:
    void do_connect (std::string);
    void on_connect (const boost::system::error_code&, tcp::endpoint*,
        tcp::socket*);

    vec_str nodes;
    std::string ip_of_this;
    NetObserver* observer = nullptr;
    boost::asio::io_service& iosvc;
    int port = 0;
};

}
}
