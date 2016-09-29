#pragma once
#include "message.hh"
#include <string>

namespace eclipse {
  namespace messages {
    struct BlockRequest: public Message {
      BlockRequest();
      BlockRequest(std::string, int);
      std::string get_type() const override;

      std::string name;
      int net_id;
    };

  }
}
