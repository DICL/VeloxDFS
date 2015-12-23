#pragma once

#include "nodelocal.hh"
#include "../network/topology.hh"
#include "../cache/cache.hh"
#include "../common/histogram.hh"

#include <string>
#include <thread>
#include <boost/asio.hpp>

namespace eclipse {

using std::string;
using std::thread;
using std::map;

class PeerLocal: public NodeLocal {
  public:
    PeerLocal ();
    ~PeerLocal ();

    bool establish() override;
    void insert (string, string);
    string lookup (string);
    bool exist (string);
    void close ();

    template <typename T> void process_message (T);

  protected:
    u_ptr<lru_cache<string, string> > cache;
    u_ptr<thread> detached;
    u_ptr<Histogram> histogram;
    u_ptr<Topology> topology;
};

}
