#pragma once
#include "message.hh"
#include <string>
#include <vector>
#include <iostream>

namespace eclipse {
  namespace messages {
    struct Boundaries: public Message {
      Boundaries() = default;
      Boundaries(std::vector<uint32_t>);
      std::string get_type() const override;

      std::vector<uint32_t> data;
    };
  }
}
