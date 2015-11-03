#pragma once

#include "node.hh"
#include "noderemote.hh"
#include "../common/logger.hh"

#include <map>
#include <memory>

namespace Nodes {

struct NodeLocal: public Node {
  NodeLocal();
  ~NodeLocal();

  std::string get_ip () const override;

  std::multimap<int, NodeRemote*> universe;
  boost::asio::io_service io_service;
  std::unique_ptr<Logger, decltype(&Logger::disconnect)> logger;
  std::string ip_of_this;
};

} /* nodes */
