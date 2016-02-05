#pragma once

#include "node.hh"
#include "noderemote.hh"
#include "../common/context.hh"

#include <map>
#include <memory>
#include <boost/asio.hpp>

namespace eclipse {

class NodeRemote;

class NodeLocal: public Node {
  friend class NodeRemote;
  public:
    NodeLocal(Context&);
    ~NodeLocal();

    std::string get_ip () const override;
    virtual bool establish() = 0;

  protected:
    std::map<int, NodeRemote*> universe;
    Logger* logger;
    std::string ip_of_this;
    boost::asio::io_service& io_service;
    int port;
};

} /* nodes */
