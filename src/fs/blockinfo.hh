#pragma once
#include <string>
#include <cstdint>

namespace eclipse {
  struct BlockInfo {
    uint32_t file_id;
    unsigned int block_seq;
    uint32_t block_hash_key;
    std::string block_name;
    uint32_t block_size;
    unsigned int is_inter;
    std::string node;
    std::string l_node;
    std::string r_node;
    unsigned int commit;
  };
}
