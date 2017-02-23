#pragma once
#include "message.hh"
#include <cstdint>

namespace eclipse {
namespace messages {
  struct BlockUpdate: public Message {
    BlockUpdate() : seq(0), hash_key(0), size(0), replica(0), pos(0), len(0), is_header(false) {};

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
