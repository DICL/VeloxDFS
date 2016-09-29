#pragma once

#include "message.hh"
#include <string>
#include <vector>
#include <iostream>
#include "nodeinfo.hh"

namespace eclipse {
namespace messages {

struct NodeList: public Message {
  NodeList() = default;
  NodeList(std::vector<NodeInfo>);

  std::string get_type() const override;
  std::vector<NodeInfo> data;
};

}
}
