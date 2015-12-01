#pragma once

#include "message.hh"
#include <string>

namespace eclipse {
namespace messages {

struct KeyValue: public Message {
  KeyValue () = default;
  KeyValue (std::string, std::string);

  std::string get_type() const override;
  std::string key, value;
};

}
}
