#pragma once
#include "executable.hh"
#include "block.hh"
#include "../nodes/fs.hh"
#include <cstdint>

namespace eclipse {
namespace messages {

struct BlockInfo: public Executable<FS> {
  std::string get_type() const override;

  Block block;
  void exec(FS*, message_fun) override; 
};

}
}
