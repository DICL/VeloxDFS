#pragma once

#include "executable.hh"
#include "../nodes/fs.hh"
#include <string>
#include <vector>
#include <iostream>
#include "fileinfo.hh"

namespace eclipse {
namespace messages {

struct FileList: public Executable<FS> {
  FileList() = default;

  List_files list_of_files;

  std::string get_type() const override;
  void exec(FS*, message_fun) override;
};

}
}
