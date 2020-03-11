#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace eclipse {
struct ChunkMetadata{
	uint32_t size;
	uint64_t offset;
	uint64_t foffset;
	uint32_t chunk_seq;

	std::string name;
	std::string primary_file;
    unsigned int primary_seq;
};

struct BlockMetadata {
  std::string name;
  std::string file_name;
  unsigned int seq;
  uint32_t hash_key;
  uint64_t size;
  unsigned int type;
  int replica;
  std::string node;
  std::string l_node;
  std::string r_node;
  unsigned int is_committed;

  std::vector<ChunkMetadata> chunks;
};

}
