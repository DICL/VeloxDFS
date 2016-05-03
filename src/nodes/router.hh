#pragma once
#include "../nodes/node.hh"
#include "../nodes/fs.hh"
#include "../messages/boost_impl.hh"
#include <functional>

namespace eclipse {
using namespace eclipse::network;

class Router: public Node, public AsyncNode {
  public:
    Router (network::Network*, FS*);
    ~Router ();

    void on_connect() override;
    void on_disconnect(int) override;
    void on_read(messages::Message*, int) override;
    void on_read_peerdfs(messages::Message*, int);
    void async_reply(messages::Message*, int);

  protected:
    FS* peer;
    Network* network;
};

} /* eclipse  */ 
