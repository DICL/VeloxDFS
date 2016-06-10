#pragma once

#include "message.hh"
#include <vector>

namespace eclipse {
namespace messages {

struct FileDescription: public Message {
  FileDescription() = default;
  ~FileDescription() = default;

  std::string get_type() const override;

  std::string name;
  uint64_t size;
  std::vector<std::string> blocks;
  std::vector<uint32_t> hash_keys;
  std::vector<uint32_t> block_size;
  int replica;
};

}
}

