#pragma once
#include "network.hh"
#include "asyncnode.hh"
#include "asyncchannel.hh"
#include "meshtopology.hh"
#include "../common/context.hh"
#include <map>

namespace eclipse {
namespace network {

class MeshNetwork: public Network {
  public:
    MeshNetwork (AsyncNode*, Context&);
    ~MeshNetwork ();

    bool establish() override;
    bool close () override;
    size_t size () override;
    bool send(int, messages::Message*) override;

  protected:
    MeshTopology* topology;
    std::map<int, Channel*> channels;
};

}
} /* eclipse  */ 
