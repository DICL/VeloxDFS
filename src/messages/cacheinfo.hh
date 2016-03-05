#pragma once

#include "message.hh"
#include <string>
#include <vector>

namespace eclipse {
namespace messages {

struct CacheInfo: public Message {
  std::string get_type() const override;
  std::vector<std::string> keys;
};

}
}
