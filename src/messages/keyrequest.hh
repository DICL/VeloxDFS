#pragma once

#include "executable.hh"
#include "../nodes/fs.hh"
#include <string>

namespace eclipse {
namespace messages {

struct KeyRequest: public Executable<FS> {
  KeyRequest () = default;
  KeyRequest (std::string);

  std::string get_type() const override;
  std::string key;
  void exec(FS*, message_fun) override;
};

}
}
