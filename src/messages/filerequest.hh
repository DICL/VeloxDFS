#pragma once

#include "message.hh"
#include <string>

namespace eclipse {
namespace messages {

struct FileRequest: public Message {
  FileRequest () : only_metadata(false) {};

  std::string get_type() const override;
  std::string name;

  bool only_metadata;
};

}
}
