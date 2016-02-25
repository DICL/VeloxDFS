#pragma once
#include <string>
#include <cstdint>

namespace eclipse {
  struct FileInfo {
    uint32_t file_id;
    std::string file_name;
    uint32_t file_hash_key;
    uint64_t file_size;
    unsigned int num_block;
    unsigned int replica;
  };
}
