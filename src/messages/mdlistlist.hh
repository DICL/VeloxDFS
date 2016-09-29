#pragma once

#include "message.hh"
#include <string>
#include <vector>
#include <iostream>
#include "mdlistinfo.hh"

namespace eclipse {
namespace messages {

struct MdlistList: public Message {
  MdlistList() = default;
  MdlistList(std::vector<MdlistInfo>);

  std::string get_type() const override;
  std::vector<MdlistInfo> data;
};

}
}
