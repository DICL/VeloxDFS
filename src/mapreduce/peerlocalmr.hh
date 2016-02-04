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
    bool insert_cache (std::string, std::string);
    bool insert_disk (std::string, std::string);

    bool lookup_cache (std::string);
    bool lookup_disk (std::string);
};

} /* eclipse  */ 
