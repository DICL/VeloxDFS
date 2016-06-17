#pragma once

#include "message.hh"
#include <string>

namespace eclipse {
namespace messages {

struct OffsetKeyValue: public Message {
  OffsetKeyValue () = default;
  OffsetKeyValue (uint32_t, std::string, std::string, uint32_t, uint32_t);

  std::string get_type() const override;
  uint32_t key;
  std::string name, value;
  uint32_t pos, len;
};

}
}
