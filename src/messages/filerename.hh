#pragma once

#include "message.hh"

namespace eclipse {
namespace messages {

struct FileRename: public Message {
  FileRename() = default;
  ~FileRename() = default;

  std::string get_type() const override;

  std::string src;
  std::string dst;
};

}
}

