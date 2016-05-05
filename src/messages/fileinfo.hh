#pragma once

#include "executable.hh"
#include "file.hh"
#include "../nodes/fs.hh"

namespace eclipse {
namespace messages {

struct FileInfo: public Executable<FS> {
  FileInfo() = default;
  ~FileInfo() = default;

  std::string get_type() const override;

  File file;
  void exec(FS*, message_fun) override;
};

}
}

