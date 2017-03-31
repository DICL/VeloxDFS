#pragma once

#include "message.hh"
#include <string>

namespace eclipse {
namespace messages {

struct BlockRequest: public Message {
  BlockRequest () = default;
  std::string get_type() const override;

  uint64_t off;
  uint64_t len;
  bool should_read_partially = false;
  std::string name;
  uint32_t hash_key;
};

}
}
