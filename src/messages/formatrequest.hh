#pragma once

#include "executable.hh"
#include "../nodes/fs.hh"
#include <string>

namespace eclipse {
namespace messages {

struct FormatRequest: public Executable<FS> {
  FormatRequest () = default;

  std::string get_type() const override;

  virtual void exec(FS* p, message_fun f) override;
};

}
}
