#pragma once
#include "message.hh"

namespace eclipse {
  namespace messages {
    struct CheckExist: public Message {
      CheckExist();
      CheckExist(std::string, std::string);
      std::string get_type() const override;
      
      std::string name;
      std::string type;
    };
  }
}

