#pragma once

#include "message.hh"
#include <string>

namespace eclipse {
namespace messages {

struct Reply: public Message {
  Reply () = default;

  std::string get_type() const override;
  std::string message;
  std::string details;
};

}
}
