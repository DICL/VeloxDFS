#pragma once

#include "message.hh"
#include <string>

namespace eclipse {
namespace messages {

struct KeyRequest: public Message {
  KeyRequest () = default;
  KeyRequest (std::string);

  std::string get_type() const override;
  std::string key;
};

}
}
