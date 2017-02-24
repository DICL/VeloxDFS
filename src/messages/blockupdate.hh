#pragma once
#include "message.hh"
#include <cstdint>

namespace eclipse {
namespace messages {
  struct BlockUpdate: public Message {
    std::string get_type() const override;

    std::string name;
    std::string file_name;
    unsigned int seq;
    uint32_t hash_key;
    uint32_t size;
    int replica;
    std::string content;
    uint32_t pos;
    uint32_t len;
    bool is_header;
  };
}
}
