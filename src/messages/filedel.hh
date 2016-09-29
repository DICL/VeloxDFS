#pragma once
#include "message.hh"

namespace eclipse {
  namespace messages {
    struct FileDel: public Message {
      FileDel();
      FileDel(std::string);
      std::string get_type() const override;

      std::string name;
    };
  }
}
