#pragma once
#include "executable.hh"
#include "../nodes/fs.hh"

namespace eclipse {
namespace messages {

struct BlockDel: public Executable<FS> {
  std::string get_type() const override;
  
  Block block;
  void exec(FS*, message_fun) override; 
};

}
}
