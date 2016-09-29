#pragma once

#include "message.hh"
#include <cstdint>

namespace eclipse {
namespace messages {

struct MdlistDel: public Message {
  MdlistDel() = default;
  ~MdlistDel() = default;
  
  std::string get_type() const override;

  std::string file_name;
  std::string slice_name;
};

}
}
