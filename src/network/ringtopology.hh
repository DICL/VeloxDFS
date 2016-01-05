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

class RingTopology: public Topology {
  public:
    using Topology::Topology;

    bool establish () override;
    bool close () override;

  private:
    void on_connect (const boost::system::error_code&,
        boost::asio::ip::tcp::resolver::iterator);
    void on_accept (const boost::system::error_code&);
    std::tuple<std::string, std::string> organize();

    std::unique_ptr<tcp::socket> client_sock;
    std::unique_ptr<tcp::socket> server_sock;
};

} /* network */ 
} /* eclipse */ 
