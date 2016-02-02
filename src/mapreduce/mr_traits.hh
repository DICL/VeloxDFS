#pragma once 
#include "../nodes/nodelocal.hh"
#include "../common/dl_loader.hh"
#include "../network/centralizedtopology.hh"

namespace eclipse {

class MR_traits: public NodeLocal {
  public:
    MR_traits(Settings&);
    ~MR_traits();

    virtual bool load(std::string) = 0;
    bool establish ();

  protected:
    DL_loader* dl_loader;
    unique_prt<CentralizedTopology> network;
};

}
