#pragma once
#include "../nodes/peer.hh"
#include "../messages/boost_impl.hh"
#include "../fs/directory.hh"

namespace eclipse {

class PeerCache: public PeerDFS {
  public:
    PeerCache (Context&);
    ~PeerCache ();

    bool insert (std::string, std::string);
    void request (string, req_func);
    bool exists (string);
    void lookup (std::string, req_func);
    bool store (messages::FileInfo*);

    int H (string);
    bool belongs (string);
    vec_str info();

    using PeerDFS::establish;
    using PeerDFS::close;

    template <typename T> void process (T);

  protected:
    u_ptr<Histogram> histogram;
    u_ptr<lru_cache<string, string> > cache;
};

} /* eclipse  */ 
