#pragma once

#include "message.hh"

namespace eclipse {
namespace messages {

struct FileInfo: public Message {
  FileInfo() = default;
  ~FileInfo() = default;

  std::string get_type() const override;

  std::string name;
  uint32_t hash_key;
  uint64_t size;
  unsigned int num_block;
  unsigned int type;
  unsigned int replica;
};

}
}

