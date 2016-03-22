#pragma once

#include "message.hh"
#include <string>

namespace eclipse {
namespace messages {

struct KeyValue: public Message {
  KeyValue () = default;
  KeyValue (uint32_t, std::string, std::string);

  std::string get_type() const override;
  uint32_t key;
  std::string name, value;
};

}
}
