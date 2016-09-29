#pragma once

#include "message.hh"
#include <string>

namespace eclipse {
  namespace messages {
    struct KeyValue: public Message {
      KeyValue () = default;
      KeyValue (int, std::string, std::string);
      std::string get_type() const override;

      int key;
      std::string name;
      std::string value;
    };
  }
}
