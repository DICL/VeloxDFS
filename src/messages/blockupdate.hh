#pragma once
#include "message.hh"
#include <cstdint>

namespace eclipse {
namespace messages {
  struct BlockUpdate: public Message {
    BlockUpdate ();
    BlockUpdate (std::string, int, uint64_t, uint64_t, int, std::string);
    std::string get_type() const override;

    std::string name;
    int net_id;
    uint64_t pos;
    uint64_t len;
    int replica;
    std::string content;
  };
}
}
