#pragma once
#include "executable.hh"
#include "../nodes/peerdfs.hh"

namespace eclipse {
namespace messages {

struct FileExist: public Executable<PeerDFS> {
  std::string get_type() const override;
  void exec(PeerDFS*, message_fun) override;

  std::string file_name;
};

}
}
