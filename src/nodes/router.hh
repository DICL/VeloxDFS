#pragma once
#include "../nodes/node.hh"
#include "../messages/boost_impl.hh"
#include <functional>

namespace eclipse {
using namespace eclipse::network;

/**
 * This class handle every incomming message.
 * It inherits from Node and implements AsyncNode
 */
class Router: public Node, public AsyncNode {
  public:
    Router (network::Network*);
    ~Router ();

    void on_connect() override;
    void on_disconnect(int) override;
    void on_read(messages::Message*, int) override;

  protected:
    std::map<std::string, std::function<void(messages::Message*, int)>> routing_table;
    std::unique_ptr<Node> peer;
    int port;
};

} /* eclipse  */ 
