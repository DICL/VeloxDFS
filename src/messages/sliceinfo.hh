#pragma once
#include "message.hh"
#include <cstdint>

namespace eclipse {
  namespace messages {
    struct SliceInfo: public Message {
      SliceInfo();
      SliceInfo(std::string, uint32_t, uint64_t, std::string&);
      SliceInfo(std::string, uint32_t, uint64_t);

      std::string get_type() const override;

      std::string name;
      uint32_t hash_key;
      uint64_t size;
      std::string content;
    };
  }
}
