#pragma once
#include "message.hh"
#include <cstdint>

namespace eclipse {
namespace messages {
  struct BlockUpdate: public Message {
    std::string get_type() const override;

    std::string name;
    std::string file_name;
    unsigned int seq = 0;
    uint32_t hash_key = 0;
    uint32_t size = 0;
    int replica = 0;
    std::string content;
    uint32_t pos = 0;
    uint32_t len = 0;
    bool is_header = false;
  };
}
}
