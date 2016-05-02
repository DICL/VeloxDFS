#pragma once
#include "executable.hh"
#include "block.hh"
#include "../nodes/peerdfs.hh"
#include <cstdint>

namespace eclipse {
namespace messages {

struct BlockInfo: public Executable<PeerDFS> {
  std::string get_type() const override;

  Block block;
  void exec(PeerDFS*, message_fun) override; 
};

}
}
