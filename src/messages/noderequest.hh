#pragma once
#include "message.hh"
#include <string>

namespace eclipse {
  namespace messages {
    struct NodeRequest: public Message {
      NodeRequest();
      NodeRequest(std::string);
      std::string get_type() const override;

      std::string name;
    };
  }
}
