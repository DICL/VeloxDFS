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
  uint32_t hash_key = 0;
  uint64_t size = 0;
  unsigned int num_block = 0;
  unsigned int n_lblock = 0;
  unsigned int type;
  unsigned int replica = 0;
  bool reducer_output = false;
  uint32_t job_id = 0;
  int uploading = 1;
  bool is_input = false;
  uint64_t intended_block_size = 0;

  std::vector<BlockMetadata> blocks_metadata;
};

}
}

