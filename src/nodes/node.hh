#pragma once

#include "machine.hh"
#include "../common/context.hh"
#include "../messages/message.hh"
#include "../network/network.hh"

#include <map>
#include <memory>
#include <boost/asio.hpp>

namespace eclipse {

class Node: public Machine {
  public:
    Node(Context&);
    ~Node();

    std::string get_ip () const override;
    virtual bool establish() = 0;

  protected:
    Logger* logger;
    network::Network* network;
    Settings& settings;
    std::string ip_of_this;
    boost::asio::io_service& io_service;
    int port;
};

} /* nodes */
