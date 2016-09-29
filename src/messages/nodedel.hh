#pragma once

#include "message.hh"

namespace eclipse {
namespace messages {

struct NodeDel: public Message {
  NodeDel() = default;
  ~NodeDel() = default;
  
  std::string get_type() const override;

  std::string name;
  std::string type;
};

}
}
