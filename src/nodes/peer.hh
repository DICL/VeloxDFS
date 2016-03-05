#pragma once

#include "node.hh"
#include "../network/asyncnode.hh"
#include "../cache/cache.hh"
#include "../common/histogram.hh"

#include <string>
#include <boost/asio.hpp>

namespace eclipse {

using std::string;
using std::thread;
using std::map;
using vec_str    = std::vector<std::string>;

typedef std::function<void(std::string)> req_func;

class Peer: public Node, public AsyncNode {
  public:
    Peer (Context&);
    ~Peer ();

    bool establish () override;
    void on_read (messages::Message*) override;
    void on_connect () override;
    void on_disconnect() override;

    void insert (string, string);
    void request (string, req_func);
    bool exists (string);
    bool belongs (string);
    void close ();

    int H (string);
    vec_str info();

  protected:
    u_ptr<lru_cache<string, string> > cache;
    u_ptr<Histogram> histogram;
    std::map<std::string, req_func> requested_blocks;
    bool connected = false;

    template <typename T> void process (T);
};

}
