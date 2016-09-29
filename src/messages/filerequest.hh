#pragma once
#include "message.hh"
#include <string>

namespace eclipse {
  namespace messages {
    struct FileRequest: public Message {
      FileRequest();
      FileRequest(std::string);
      std::string get_type() const override;

      std::string name;
    };
  }
}
