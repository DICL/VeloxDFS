#pragma once
#include "../common/peerlocal.hh"

namespace eclipse {

class PeerLocalMR: public PeerLocal {
  public:
    PeerLocalMR (Context&);
    ~PeerLocalMR ();

    bool insert (std::string, std::string);
    std::string lookup (std::string);

  private:
    FileManager f_manager;
};

} /* eclipse  */ 
