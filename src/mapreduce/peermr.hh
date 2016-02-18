#pragma once
#include "../nodes/peer.hh"

namespace eclipse {

class PeerMR: public Peer {
  public:
    PeerMR (Context&);
    ~PeerMR ();

    bool insert (std::string, std::string);
    void lookup (std::string, req_func);

    using Peer::establish;
    using Peer::close;
  private:
};

} /* eclipse  */ 
