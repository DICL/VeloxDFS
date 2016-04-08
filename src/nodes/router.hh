#pragma once
#include "../nodes/node.hh"
#include "../messages/boost_impl.hh"
#include "../network/asyncnetwork.hh"
#include "../network/server.hh"
#include <functional>

namespace eclipse {

using boost::system::error_code;
using boost::asio::ip::tcp;
using namespace eclipse::network;

class Router: public Node, public AsyncNode {
  public:
    Router (Context&);
    ~Router ();

    bool establish() override; 
    void on_connect() override;
    void on_disconnect() override;
    void on_read(messages::Message*) override;

  protected:
    std::map<std::string, std::function<void(messages::Message*)>> routing_table;
    int port;
};

} /* eclipse  */ 
