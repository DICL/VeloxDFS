#pragma once
#include "../nodes/peerdfs.hh"
#include "../messages/boost_impl.hh"
#include <functional>

namespace eclipse {
using namespace eclipse::network;

class Router: public Node, public AsyncNode {
  public:
    Router (network::Network*, PeerDFS*);
    ~Router ();

    void on_connect() override;
    void on_disconnect(int) override;
    void on_read(messages::Message*, int) override;
    void on_read_peerdfs(messages::Message*, int);
    void async_reply(messages::Message*, int);

  protected:
    PeerDFS* peer;
    Network* network;
};

} /* eclipse  */ 
