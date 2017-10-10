#pragma once

#include "fileinfo.hh"
#include "../common/logical_block_metadata.hh"
#include <vector>

namespace eclipse {
namespace messages {

struct FileDescription: public FileInfo {
  FileDescription() = default;
  ~FileDescription() = default;

  FileDescription& operator=(FileDescription&);

  std::string get_type() const override;

  std::vector<std::string> blocks;
  std::vector<uint32_t> hash_keys;
  std::vector<uint64_t> block_size;
  std::vector<std::string> block_hosts;
  std::vector<logical_block_metadata> logical_blocks; // TODO, refactor this hell
};

}
}

