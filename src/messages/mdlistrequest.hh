#pragma once

#include "message.hh"
#include <string>

namespace eclipse {
namespace messages {

struct MdlistRequest: public Message {
  MdlistRequest () = default;

  std::string get_type() const override;
  std::string name;
};

}
}
