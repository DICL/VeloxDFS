#pragma once
#include "message.hh"
#include <cstdint>

namespace eclipse {
  namespace messages {
    struct NodeUpdate: public Message {
      std::string get_type() const override;
      std::string name;
      std::string type;
      int id;
      int net_id;
    };
  }
}
