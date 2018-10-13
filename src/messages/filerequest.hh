#pragma once

#include "message.hh"
#include <string>

namespace eclipse {
namespace messages {

struct FileRequest: public Message {
  FileRequest () = default;

  std::string get_type() const override;
  std::string name;
  std::string type;

  bool generate = false;
};

}
}
