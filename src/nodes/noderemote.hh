#pragma once

#include "node.hh"
#include "../messages/message.hh"

#include <memory>
#include <boost/asio.hpp>

namespace Nodes {
using namespace network;
using namespace boost::asio;
using boost::asio::ip::tcp;

class NodeRemote: public Node {
  public:
    NodeRemote(io_service&, std::string, int, int);
    ~NodeRemote() = default;

    bool connect() ;
    void close() ;
    std::string get_ip() const override;

    virtual void send (Message*) = 0;

  protected:
    io_service& ioservice;

    u_ptr<tcp::socket> socket;
    std::string host;
    int port;
};
}
