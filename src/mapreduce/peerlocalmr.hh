#pragma once
#include "../nodes/peerlocal.hh"

namespace eclipse {

class PeerLocalMR: public PeerLocal {
  public:
    PeerLocalMR (Context&);
    ~PeerLocalMR ();

    bool insert (std::string, std::string);
    void lookup (std::string, req_func);

  private:
};

} /* eclipse  */ 
