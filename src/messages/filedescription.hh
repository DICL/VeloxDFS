#pragma once

#include "fileinfo.hh"
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
};

}
}

