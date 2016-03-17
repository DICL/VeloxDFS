#pragma once

#include "message.hh"
#include <string>

namespace eclipse {
namespace messages {

struct FormatRequest: public Message {
  FormatRequest () = default;

  std::string get_type() const override;
};

}
}
