#pragma once

#include "executable.hh"
#include "../nodes/peerdfs.hh"
#include <string>
#include <vector>
#include <iostream>
#include "fileinfo.hh"

namespace eclipse {
namespace messages {

struct FileList: public Executable<PeerDFS> {
  FileList() = default;

  List_files list_of_files;

  std::string get_type() const override;
  void exec(PeerDFS*, message_fun) override;
};

}
}
