#pragma once

#include "message.hh"
#include <string>

namespace eclipse {
namespace messages {

struct SliceRequest: public Message {
  SliceRequest () = default;

  std::string get_type() const override;
  std::string name;
  std::string hash_key;
};

}
}
