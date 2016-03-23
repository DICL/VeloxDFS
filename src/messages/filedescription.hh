#pragma once

#include "message.hh"
#include <vector>

namespace eclipse {
namespace messages {

struct FileDescription: public Message {
  FileDescription() = default;
  ~FileDescription() = default;

  std::string get_type() const override;

  std::string file_name;
  std::vector<std::string> blocks;
  std::vector<uint32_t> hashes;
};

}
}

