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
      
      // TODO: information about replicas
      //int num_replicas;
      //std::vector<std::string> replica_hosts;
      //
      
      block_metadata() = default;
      
      block_metadata(const block_metadata& that) {
        this->name = that.name;
        this->size = that.size;
        this->host = that.host;
        this->index = that.index;
        this->file_name = that.file_name;
      };

      block_metadata& operator=(const block_metadata& rhs) {
        this->name = rhs.name;
        this->size = rhs.size;
        this->host = rhs.host;
        this->index = rhs.index;
        this->file_name = rhs.file_name;

        return *this;
      };
    };
  }
}

#endif
