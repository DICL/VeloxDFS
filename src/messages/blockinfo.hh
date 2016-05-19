#pragma once
#include "message.hh"
#include <cstdint>

namespace eclipse {
namespace messages {
  struct BlockInfo: public Message {
    std::string get_type() const override;

    std::string name;
    std::string file_name;
    unsigned int seq;
    uint32_t hash_key;
    uint32_t size;
    unsigned int type;
    int replica;
    std::string node;
    std::string l_node;
    std::string r_node;
    unsigned int is_committed;
    std::string content;
  };
}
}
