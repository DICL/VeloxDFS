#pragma once

#include "message.hh"
#include <string>
#include <vector>
#include <iostream>

namespace network {

struct Boundaries: public Message {
  Boundaries() = default;
  Boundaries(std::vector<uint64_t>);

  std::vector<uint64_t> data;
  std::string get_type() const override;
};

}
