#pragma once
#include "message.hh"
#include <cstdint>

namespace eclipse {
namespace messages {

struct BlockStatus: public Message {
  std::string get_type() const override;

  std::string name;
  uint32_t hash_key;
  bool success = false;
};

}
}
