#pragma once
#include "executable.hh"
#include "../nodes/fs.hh"

namespace eclipse {
namespace messages {

struct FileExist: public Executable<FS> {
  std::string get_type() const override;
  void exec(FS*, message_fun) override;

  std::string file_name;
};

}
}
