#pragma once

#include <string>
#include <vector>
#include <stdint.h>

#include "block_metadata.hh"

namespace velox {
namespace model {

class metadata {
  public:
    metadata() = default;

    std::string name;
    uint32_t hash_key = 0;
    uint64_t size = 0;
    unsigned int num_block = 0;
    unsigned int num_chunks = 0;
    unsigned int num_static_blocks = 0;
    unsigned int type;
    unsigned int replica;
    bool has_block_data = true;
	int lbm_id;

    std::vector<std::string> blocks;
    std::vector<uint32_t> hash_keys;
    std::vector<uint64_t> block_size;
    std::vector<block_metadata> block_data;
};

}
}
