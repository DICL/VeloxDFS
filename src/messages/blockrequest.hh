#pragma once

#include "message.hh"
#include <string>

namespace eclipse {
namespace messages {

struct BlockRequest: public Message {
  BlockRequest () = default;

  std::string get_type() const override;
  uint32_t block_hash_key;
};

}
}
