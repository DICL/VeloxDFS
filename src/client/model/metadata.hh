#ifndef __MODEL_METADATA_HH__
#define __MODEL_METADATA_HH__

#include <string>
#include <vector>
#include <stdint.h>

#include "block_metadata.hh"

namespace velox {
  namespace model {
    struct metadata {
      std::string name;
      uint32_t hash_key;
      uint64_t size;
      unsigned int num_block;
      unsigned int type;
      unsigned int replica;
      std::vector<std::string> blocks;
      std::vector<uint32_t> hash_keys;
      std::vector<uint64_t> block_size;
      std::vector<block_metadata> block_data;
    };
  }
}

#endif
