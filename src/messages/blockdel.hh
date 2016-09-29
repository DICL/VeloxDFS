#pragma once
#include <vector>
#include <cstdint>
#include "message.hh"

namespace eclipse {
  namespace messages {
    struct BlockDel: public Message {
      BlockDel();
      BlockDel(std::string);
      std::string get_type() const override;

      std::string name;
    };
  }
}
