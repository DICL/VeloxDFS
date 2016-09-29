#pragma once
#include "message.hh"
#include <cstdint>

namespace eclipse {
  namespace messages {
    struct MdlistInfo: public Message {
      MdlistInfo();
      MdlistInfo(std::string, std::string, uint64_t, uint64_t, uint64_t);
      std::string get_type() const override;

      std::string file_name;
      std::string slice_name;
      uint64_t file_pos;
      uint64_t slice_pos;
      uint64_t len;
    };
  }
}
