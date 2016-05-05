#pragma once

#include "executable.hh"
#include "../nodes/fs.hh"
#include <string>

namespace eclipse {
namespace messages {

struct FileRequest: public Executable<FS> {
  FileRequest () = default;

  std::string get_type() const override;
  std::string file_name;
  void exec(FS*, message_fun) override;

};

}
}
