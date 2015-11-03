#pragma once

#include "node.hh"
#include "../messages/message.hh"

#include <memory>
#include <boost/asio.hpp>


namespace Nodes {
  class NodeRemote: public Node {
    public:
    typedef boost::asio::io_service io_ptr;

    NodeRemote(io_ptr&, std::string, int, int);
    ~NodeRemote() = default;

    bool connect() ;
    void close() ;
    virtual void send(network::Message*) = 0;
    virtual std::string get_ip() const override;

    protected:
//    io_ptr io_service;
    boost::asio::io_service&    ioservice;

    std::unique_ptr<boost::asio::ip::tcp::socket> socket;
    std::string host;
    int port;

  };
}
