#pragma once

#include "message.hh"

namespace eclipse {
namespace messages {

struct FileDel: public Message {
  FileDel() = default;
  ~FileDel() = default;
  
  std::string get_type() const override;

  std::string name;
};

}
}
