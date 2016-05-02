#pragma once

#include "executable.hh"
#include "../nodes/peerdfs.hh"
#include <string>

namespace eclipse {
namespace messages {

struct FileRequest: public Executable<PeerDFS> {
  FileRequest () = default;

  std::string get_type() const override;
  std::string file_name;
  void exec(PeerDFS*, message_fun) override;

};

}
}
