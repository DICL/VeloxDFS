#ifndef __MODEL_BLOCK_METADATA_HH__
#define __MODEL_BLOCK_METADATA_HH__

#include <string>
#include <stdint.h>

namespace velox {
  namespace model {
    struct block_metadata {
      std::string name;
      uint64_t size;
      std::string host;
      int index;
      std::string file_name;
    };
  }
}

#endif
