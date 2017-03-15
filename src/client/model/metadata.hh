#ifndef __MODEL_METADATA_HH__
#define __MODEL_METADATA_HH__

#include <string>
#include <vector>
#include <stdint.h>

namespace velox {
  namespace model {
    struct metadata {
      std::string name;
      uint32_t hash_key;
      uint32_t size;
      unsigned int num_block;
      unsigned int type;
      unsigned int replica;
      std::vector<std::string> blocks;
      std::vector<uint32_t> hash_keys;
      std::vector<uint32_t> block_size;
    };

  }
}

#endif
