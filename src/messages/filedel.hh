#pragma once

#include "executable.hh"
#include "reply.hh"
#include "../nodes/fs.hh"

namespace eclipse {
namespace messages {

struct FileDel: public Executable<FS> {
  FileDel() = default;
  ~FileDel() = default;
  
  std::string get_type() const override;

  std::string file_name;

  void exec(FS*, message_fun) override;
};

}
}
