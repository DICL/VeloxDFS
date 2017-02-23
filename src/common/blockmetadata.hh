#pragma once
#include <cstdint>
#include <string>

namespace eclipse {

struct BlockMetadata {
  std::string name;
  std::string file_name;
  unsigned int seq;
  uint32_t hash_key;
  uint32_t size;
  unsigned int type;
  int replica;
  std::string node;
  std::string l_node;
  std::string r_node;
  unsigned int is_committed;
};

}
