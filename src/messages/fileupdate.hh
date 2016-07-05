#pragma once

#include "message.hh"

namespace eclipse {
namespace messages {

struct FileUpdate: public Message {
  FileUpdate() = default;
  ~FileUpdate() = default;

  std::string get_type() const override;

  std::string name;
  uint64_t size;
  unsigned int num_block;
};

}
}

