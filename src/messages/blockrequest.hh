#pragma once

#include "executable.hh"
#include "../nodes/peerdfs.hh"
#include <string>

namespace eclipse {
namespace messages {

struct BlockRequest: public Executable<PeerDFS> {
  BlockRequest () = default;

  std::string get_type() const override;
  std::string block_name;
  uint32_t hash_key;
  void exec(PeerDFS*, message_fun) override; 
  void on_exec(std::string, std::string, message_fun); 
};

}
}
