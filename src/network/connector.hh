#pragma once
#include "netobserver.hh"
#include "../common/context.hh"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/spawn.hpp>
#include <memory>

namespace eclipse {
namespace network {

using

class Connector {
  public:
    Connector(Context&, int, NetObserver*); 
    ~Connector(); 

    void establish ();

  protected:
    void do_connect ();
    void on_connect ();

    vec_str& nodes;
    NetObserver* observer = nullptr;
    boost::asio::io_service& iosvc;
    int port = 0;
};

}
};
