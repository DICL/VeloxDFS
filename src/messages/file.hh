#pragma once

#include "message.hh"
#include "mapreduce/task.hh"

namespace eclipse {
namespace messages {

struct File: public Message {
  File() = default;
  ~File() = default;

  std::string get_type() const override;
  std::string filename;
};

}
}

