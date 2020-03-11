#ifndef __MODEL_BLOCK_METADATA_HH__
#define __MODEL_BLOCK_METADATA_HH__

#include <string>
#include <stdint.h>
#include <vector>

namespace velox {
  namespace model {
    class block_metadata { //logical_block
      public:
      std::string name; //Chunk_name
      uint64_t size;
      std::string host;
      int index; // chunk_seq
      int primary_chunk_num; // chunk_seq
      std::string file_name;

			std::string primary_file; // added
			uint64_t offset; // added
			uint64_t foffset; // added
			uint32_t chunk_seq; // added
			uint64_t primary_seq; // added

      std::vector<block_metadata> chunks; //physical_blocks
    };
  }
}

#endif
