#pragma once

#include "topology.hh"

#include <boost/asio.hpp>
#include <boost/asio/error.hpp>
#include <memory>
#include <tuple>

namespace eclipse {
namespace network {

using boost::asio::ip::tcp;
using namespace boost::asio;

class MeshTopology: public Topology {
  public:
    using Topology::Topology;

    bool establish () override;
    bool close () override;

  private:
    int net_size, clients_connected = 1;

    void on_connect (const boost::system::error_code&,
        tcp::socket*, tcp::endpoint*);

    void on_accept (const boost::system::error_code&,
        tcp::socket*);

    std::unique_ptr<tcp::acceptor> acceptor;
    std::vector< std::unique_ptr<tcp::socket> > servers_sock;
    std::vector< u_ptr<tcp::socket> > clients_sock;
};

} /* network */ 
} /* eclipse */ 
