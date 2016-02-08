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

typedef std::function<void(std::string)> req_func;

class PeerLocal: public NodeLocal {
  public:
    PeerLocal (Context&);
    ~PeerLocal ();

    bool establish() override;
    void process_message (messages::Message*) override;
    void insert (string, string);
    void request (string, req_func);
    bool exists (string);
    bool belongs (string);
    void close ();

    void run ();
    void join ();

    int H(string);

  protected:
    u_ptr<lru_cache<string, string> > cache;
    u_ptr<Histogram> histogram;
    u_ptr<Topology> topology;
    std::vector<u_ptr<std::thread>> threads;
    std::map<std::string, req_func> requested_blocks;
    int concurrency;

    template <typename T> void process (T);
};

}
