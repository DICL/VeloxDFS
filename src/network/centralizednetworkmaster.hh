#pragma once
#include "network.hh"

namespace eclipse {
namespace network {

class CentralizedNetworkMaster: public Network {
  public:
    CentralizedNetworkMaster ();
    ~CentralizedNetworkMaster ();

    bool establish() override;
    bool close () override;
    size_t size () override;
    bool send(int, messages::Message*) override;

  proctected:
    StarTopology* topology = nullptr;
    Channel* client = nullptr;
};

}
} /* eclipse  */ 
