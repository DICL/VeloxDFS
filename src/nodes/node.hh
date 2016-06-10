#pragma once

#include "machine.hh"
#include "../common/context_singleton.hh"
#include "../messages/message.hh"
#include "../network/network.hh"

#include <map>
#include <memory>
#include <boost/asio.hpp>

namespace eclipse {

class Node: public Machine {
  public:
    Node();
    ~Node();

    std::string get_ip () const override;

  protected:
    network::Network* network;
    Logger* logger;
    std::string ip_of_this;
    int port;
};

} /* nodes */
