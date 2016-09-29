#pragma once

#include "message.hh"

namespace eclipse {
namespace messages {

struct MdlistUpdate: public Message {
  MdlistUpdate() = default;
  ~MdlistUpdate() = default;

  std::string get_type() const override;

  std::string file_name;
  std::string slice_name;
  uint64_t len;
};

}
}

