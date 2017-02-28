#pragma once

#include "message.hh"
#include <string>

namespace eclipse {
namespace messages {

struct BlockRequest: public Message {
  BlockRequest () = default;
  std::string get_type() const override;

  uint32_t off;
  uint32_t len;
  bool should_read_partially = false;
  std::string name;
  uint32_t hash_key;
};

}
}
