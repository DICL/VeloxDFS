#pragma once

#include "message.hh"
#include <string>
#include <vector>
#include <iostream>
#include "sliceinfo.hh"

namespace eclipse {
namespace messages {

struct SliceList: public Message {
  SliceList() = default;
  SliceList(std::vector<SliceInfo>);

  std::string get_type() const override;
  std::vector<SliceInfo> data;
};

}
}
