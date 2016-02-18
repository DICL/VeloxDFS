#pragma once
#include <string>
#include <cstdint>

struct BlockInfo {
  uint32_t file_id;
  uint32_t block_seq;
  std::string block_name;
  std::string real_name;
  uint64_t block_size;
  bool is_inter;
  std::string node;
  std::string l_node; // NULL is ok
  std::string r_node; // NULL is ok
  bool commit; // NULL is ok
};
