#pragma once

#include "message.hh"

namespace eclipse {
namespace messages {

struct File: public Message {
  File() = default;
  ~File() = default;

  std::string get_type() const override;

  std::string file_name;
  uint32_t file_hash_key = 0;
  uint64_t file_size = 0;
  unsigned int num_block = 0;
  unsigned int replica = 0;
};

}
}

