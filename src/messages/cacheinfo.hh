#pragma once

#include "executable.hh"
#include "../nodes/peerdfs.hh"
#include <string>
#include <vector>

namespace eclipse {
namespace messages {

struct CacheInfo: public Executable<PeerDFS> {
  std::string get_type() const override;
  std::vector<std::string> keys;

  void exec(PeerDFS* p, message_fun) override { }
};

}
}
