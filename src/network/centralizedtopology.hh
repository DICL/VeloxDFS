#pragma once

#include "topology.hh"
#include "../mapreduce/mr_traits.hh"

#include <boost/asio.hpp>
#include <boost/asio/error.hpp>
#include <memory>
#include <tuple>

namespace eclipse {
class MR_traits;

namespace network {

using boost::asio::ip::tcp;
using namespace boost::asio;

class CentralizedTopology: public Topology {
  public:
    CentralizedTopology(eclipse::MR_traits*,
        boost::asio::io_service&, Logger*,
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
    void dummy_callback (const boost::system::error_code&);

    std::unique_ptr<tcp::acceptor> acceptor;
    eclipse::MR_traits* owner;
};

} /* network */ 
} /* eclipse */ 
