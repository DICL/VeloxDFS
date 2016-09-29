#pragma once

#include "message.hh"
#include <cstdint>

namespace eclipse {
namespace messages {

struct SliceDel: public Message {
  SliceDel() = default;
  ~SliceDel() = default;
  
  std::string get_type() const override;

  std::string name;
};

}
}
