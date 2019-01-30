#ifndef __MODEL_BLOCK_METADATA_HH__
#define __MODEL_BLOCK_METADATA_HH__

#include <string>
#include <stdint.h>
#include <vector>

namespace velox {
  namespace model {
    class block_metadata {
      public:
      std::string name;
      uint64_t size;
      std::string host;
      int index;
      std::string file_name;
      std::vector<block_metadata> chunks;
    };
  }
}

#endif
