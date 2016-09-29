#pragma once
#include "message.hh"
#include "nodeinfo.hh"
#include <vector>

namespace eclipse {
  namespace messages {
    struct NodeDescription: public Message {
      std::string get_type() const override;

      std::vector<NodeInfo> node_list;
    };
  }
}

