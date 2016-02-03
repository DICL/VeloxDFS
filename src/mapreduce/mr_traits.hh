#pragma once 
#include "../nodes/nodelocal.hh"
#include "../common/dl_loader.hh"
#include "../network/centralizedtopology.hh"

#include  <string>
#include  <boost/asio.hpp>

namespace eclipse {
namespace network {
class CentralizedTopology;
}
class MR_traits: public NodeLocal {
  public:
    MR_traits(Settings&);
    ~MR_traits();

    bool establish () override;
    virtual void action (boost::asio::ip::tcp::socket*) = 0;

  protected:
    DL_loader* dl_loader;
    u_ptr<eclipse::network::CentralizedTopology> network;
};

}
