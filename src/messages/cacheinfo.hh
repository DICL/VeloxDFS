#pragma once

#include "executable.hh"
#include "../nodes/fs.hh"
#include <string>
#include <vector>

namespace eclipse {
namespace messages {

struct CacheInfo: public Executable<FS> {
  std::string get_type() const override;
  std::vector<std::string> keys;

  void exec(FS* p, message_fun) override { }
};

}
}
