#pragma once

#include "node.hh"
#include "nodelocal.hh"
#include "../messages/message.hh"

#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/error.hpp>

namespace eclipse {

using namespace messages;
class NodeLocal; // <- Forward initialization

class NodeRemote: public Node {
  public:
    NodeRemote(NodeLocal*);
    NodeRemote(NodeLocal*, int, std::string, int);
    ~NodeRemote() = default;

    bool do_connect();
    void close();
    std::string get_ip() const override;

  protected:
    virtual void on_connect (boost::system::error_code&) = 0;

    NodeLocal& owner;
    boost::asio::io_service& ioservice;

    u_ptr<boost::asio::ip::tcp::socket> socket;
    std::string host;
    int port;
};
}
