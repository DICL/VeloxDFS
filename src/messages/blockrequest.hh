#pragma once

#include "message.hh"
#include <string>

namespace eclipse {
namespace messages {

struct BlockRequest: public Message {
  uint32_t off;
  uint32_t len;
  bool should_read_partially;

  std::string name;
  uint32_t hash_key;

  BlockRequest () : should_read_partially(false) {};

  std::string get_type() const override;
};

}
}
