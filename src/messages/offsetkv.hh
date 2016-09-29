#pragma once
#include "message.hh"
#include <string>

namespace eclipse {
  namespace messages {
    struct OffsetKeyValue: public Message {
      OffsetKeyValue () = default;
      OffsetKeyValue (int, std::string, std::string, uint64_t, uint64_t);

      std::string get_type() const override;
      int key;
      std::string name, value;
      uint64_t pos, len;
    };
  }
}
