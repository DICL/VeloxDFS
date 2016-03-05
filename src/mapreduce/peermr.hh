#pragma once
#include "../nodes/peer.hh"
#include "../messages/boost_impl.hh"
#include "../fs/directory.hh"

namespace eclipse {

class PeerMR: public Peer {
  public:
    PeerMR (Context&);
    ~PeerMR ();

    bool insert (std::string, std::string);
    void lookup (std::string, req_func);
    bool store (messages::FileInfo*);
    bool insert_block (messages::BlockInfo*);

    using Peer::establish;
    using Peer::close;
  private:
    Directory directory;
};

} /* eclipse  */ 
