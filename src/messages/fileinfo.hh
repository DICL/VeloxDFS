#pragma once

#include "executable.hh"
#include "file.hh"
#include "../nodes/peerdfs.hh"

namespace eclipse {
namespace messages {

struct FileInfo: public Executable<PeerDFS> {
  FileInfo() = default;
  ~FileInfo() = default;

  std::string get_type() const override;

  File file;
  void exec(PeerDFS*, message_fun) override;
};

}
}

