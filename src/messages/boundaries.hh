#pragma once

#include "executable.hh"
#include "../nodes/peerdfs.hh"
#include <string>
#include <vector>
#include <iostream>

namespace eclipse {
namespace messages {

struct Boundaries: public Executable<PeerDFS> {
  Boundaries() = default;
  Boundaries(std::vector<uint64_t>);

  std::vector<uint64_t> data;
  std::string get_type() const override;

  void exec(PeerDFS* p, message_fun) override { }
};

}
}
