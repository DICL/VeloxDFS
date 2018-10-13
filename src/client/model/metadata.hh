#ifndef __MODEL_METADATA_HH__
#define __MODEL_METADATA_HH__

#include <string>
#include <vector>
#include <stdint.h>

#include "block_metadata.hh"

namespace velox {
  namespace model {
    class metadata {
      public:
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
      bool has_block_data = true;

      metadata() = default;

      metadata(const metadata& that) {
        this->name = that.name;
        this->hash_key = that.hash_key;
        this->size = that.size;
        this->num_block = that.num_block;
        this->type = that.type;
        this->replica = that.replica;
        this->has_block_data = that.has_block_data;

        if(this->has_block_data) {
          this->blocks.clear();
          for(auto block : that.blocks) 
            this->blocks.push_back(block);

          this->hash_keys.clear();
          for(auto hash_key : that.hash_keys) 
            this->hash_keys.push_back(hash_key);

          this->block_size.clear();
          for(auto bs : that.block_size) 
            this->block_size.push_back(bs);

          this->block_data.clear();
          for(auto bd : that.block_data) {
            block_metadata new_data(bd);
            this->block_data.push_back(std::move(new_data));
          }
        }
      };
      
      metadata& operator=(const metadata& rhs) {
        this->name = rhs.name;
        this->hash_key = rhs.hash_key;
        this->size = rhs.size;
        this->num_block = rhs.num_block;
        this->type = rhs.type;
        this->replica = rhs.replica;
        this->has_block_data = rhs.has_block_data;

        if(this->has_block_data) {
          this->blocks.clear();
          for(auto block : rhs.blocks) 
            this->blocks.push_back(block);

          this->hash_keys.clear();
          for(auto hash_key : rhs.hash_keys) 
            this->hash_keys.push_back(hash_key);

          this->block_size.clear();
          for(auto bs : rhs.block_size) 
            this->block_size.push_back(bs);

          this->block_data.clear();
          for(auto bd : rhs.block_data) {
            block_metadata new_data(bd);
            this->block_data.push_back(std::move(new_data));
          }
        }

        return *this;
      };

      ~metadata() {
        this->block_data.clear();
      }
    };
  }
}

#endif
