#pragma once
#include "message.hh"

namespace eclipse {
namespace messages {

struct BlockDel: public Message {
  std::string get_type() const override;
  
  std::string name;
  std::string file_name;
  unsigned int seq;
  uint32_t hash_key;
  int replica;
};

}
}
