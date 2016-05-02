#pragma once

#include "executable.hh"
#include "reply.hh"
#include "../nodes/peerdfs.hh"

namespace eclipse {
namespace messages {

struct FileDel: public Executable<PeerDFS> {
  FileDel() = default;
  ~FileDel() = default;
  
  std::string get_type() const override;

  std::string file_name;

  void exec(PeerDFS*, message_fun) override;
};

}
}
