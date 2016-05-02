#pragma once
#include "executable.hh"
#include "../nodes/peerdfs.hh"

namespace eclipse {
namespace messages {

struct BlockDel: public Executable<PeerDFS> {
  std::string get_type() const override;
  
  Block block;
  void exec(PeerDFS*, message_fun) override; 
};

}
}
