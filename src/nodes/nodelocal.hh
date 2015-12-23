#pragma once

#include "node.hh"
#include "noderemote.hh"
#include "../common/logger.hh"

#include <map>
#include <memory>
#include <boost/asio.hpp>

namespace eclipse {
class NodeRemote;

class NodeLocal: public Node {
  friend class NodeRemote;
  public:
    NodeLocal();
    ~NodeLocal();

    std::string get_ip () const override;
    virtual bool establish() = 0;

  protected:
    int port;
    std::multimap<int, NodeRemote*> universe;
    boost::asio::io_service io_service;
    std::unique_ptr<Logger, decltype(&Logger::disconnect)> logger {nullptr, Logger::disconnect};
    std::string ip_of_this;
};

} /* nodes */
