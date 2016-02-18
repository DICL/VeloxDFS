#pragma once
#include <string>
#include <cstdint>

struct FileInfo {
  uint32_t file_id;
  std::string file_name;
  uint64_t file_size;
  unsigned int num_block;
  unsigned int replica;
};
