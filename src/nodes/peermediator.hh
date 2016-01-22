#pragma once

#include "peerremote.hh"
#include "peerlocal.hh"

namespace eclipse {

class PeerMediator {
  public:
    PeerMediator(PeerRemote*, PeerLocal*);
    void set_boundaries(message::Boundaries*);

  protected:
    PeerRemote& remote;
    PeerLocal& local;
};


}
