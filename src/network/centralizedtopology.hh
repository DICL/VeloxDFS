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
    CentralizedTopology(Context&, Channel*, AsyncNode*);

    bool establish () override;
    bool close () override;
    bool is_online() override;

  private:
//    void on_connect (const boost::system::error_code&,
//        tcp::socket*, tcp::endpoint*);

    void on_accept (const boost::system::error_code&,
        tcp::socket*);

    std::unique_ptr<tcp::acceptor> acceptor;
    Channel* channel;
    AsyncNode* node;
};

} /* network */ 
} /* eclipse */ 
