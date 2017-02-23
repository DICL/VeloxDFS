#ifndef __MODEL_METADATA_HH__
#define __MODEL_METADATA_HH__

#include <string>

namespace velox {
  namespace model {
    struct metadata {
      std::string name;
      uint32_t hash_key;
      uint32_t size;
      unsigned int num_block;
      unsigned int type;
      unsigned int replica;
    };

  }
}

#endif
