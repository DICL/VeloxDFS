#pragma once

#include "message.hh"

namespace eclipse {
namespace messages {

struct FileInfo: public Message {
  FileInfo() = default;
  ~FileInfo() = default;

  std::string get_type() const override;

  std::string file_name;
  uint32_t file_hash_key;
  uint64_t file_size;
  unsigned int num_block;
  unsigned int replica;
};

}
}

