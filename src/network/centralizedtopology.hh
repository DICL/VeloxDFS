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

class CentralizedTopology: public Topology {
  public:
    CentralizedTopology(boost::asio::io_service&, Logger*,
        std::string, vec_str, int, int);

    bool establish () override;
    bool close () override;
    bool is_online() override;

  private:
    std::string type, master;
    int net_size, clients_connected = 1; 

    void on_connect (const boost::system::error_code&,
        tcp::socket*, tcp::endpoint*);

    void on_accept (const boost::system::error_code&,
        tcp::socket*);

    std::unique_ptr<tcp::acceptor> acceptor;
};

} /* network */ 
} /* eclipse */ 
