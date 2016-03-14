#pragma once

#include "message.hh"

namespace eclipse {
namespace messages {

struct FileDel: public Message {
  FileDel() = default;
  ~FileDel() = default;
  
  std::string get_type() const override;

  std::string file_name;
  //uint32_t file_id;
  //uint32_t file_hash_key;
  //uint64_t file_size;
  //unsigned int num_block;
  //unsigned int replica;
};

}
}
