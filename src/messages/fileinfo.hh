#pragma once

#include "message.hh"
#include "../common/blockmetadata.hh"
#include <vector>

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
  unsigned int n_lblock;
  unsigned int type;
  unsigned int replica;
  int uploading = 1;

  std::vector<BlockMetadata> blocks_metadata;
};

}
}

