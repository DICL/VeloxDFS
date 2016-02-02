#pragma once 
#include "dl_loader.hh"


namespace eclipse {

class MR_traits: public nodelocal {
  public:
    MR_traits();
    ~MR_traits();

    virtual bool load(std::string) = 0;

  protected:
    DL_loader* dl_loder;
    unique_prt<CentralizedTopology> network;
};

}
