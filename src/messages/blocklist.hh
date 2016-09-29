#pragma once

#include "message.hh"
#include <string>
#include <vector>
#include <iostream>
#include "blockinfo.hh"

namespace eclipse {
namespace messages {

struct BlockList: public Message {
  BlockList() = default;
  BlockList(std::vector<BlockInfo>);

  std::string get_type() const override;
  std::vector<BlockInfo> data;
};

}
}
