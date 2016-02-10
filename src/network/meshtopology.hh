#pragma once

#include "topology.hh"
#include "asyncnode.hh"

#include <boost/asio.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/spawn.hpp>
#include <memory>
#include <tuple>

namespace eclipse {
namespace network {

using boost::asio::ip::tcp;
using namespace boost::asio;

class MeshTopology: public Topology {
  public:
    MeshTopology(Context&, std::map<int, Channel*>*, AsyncNode*);

    bool establish () override;
    bool close () override;
    bool is_online() override;

  protected:
    int net_size, clients_connected = 1, server_connected = 1;
    AsyncNode* node;
    std::map<int, Channel*>* channels;

    void on_connect (const boost::system::error_code&,
        tcp::socket*, tcp::endpoint*);

    void accept (boost::asio::yield_context);

    std::unique_ptr<tcp::acceptor> acceptor;
};

} /* network */ 
} /* eclipse */ 
