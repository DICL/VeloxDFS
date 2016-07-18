#pragma once
#include "message.hh"

namespace eclipse {
namespace messages {
  struct MetaData: public Message {
    std::string get_type() const override;

    std::string name;
    std::string content;
    std::string node;
  };
}
}
