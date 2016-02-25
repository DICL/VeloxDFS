#pragma once

#include "message.hh"
#include "mapreduce/task.hh"

namespace eclipse {
namespace messages {

struct FileInfo: public Message {
  FileInfo() = default;
  ~FileInfo() = default;

  std::string get_type() const override;
  std::string filename;
  size_t size, block_number;
  uint32_t id;
};

}
}

