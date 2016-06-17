#pragma once
#include "message.hh"
#include <cstdint>

namespace eclipse {
namespace messages {
  struct BlockUpdate: public Message {
    std::string get_type() const override;

    std::string name;
    uint32_t hash_key;
    int replica;
    uint32_t pos;
    uint32_t len;
    std::string content;
  };
}
}
