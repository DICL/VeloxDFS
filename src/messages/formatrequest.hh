#pragma once

#include "executable.hh"
#include "../nodes/peerdfs.hh"
#include <string>

namespace eclipse {
namespace messages {

struct FormatRequest: public Executable<PeerDFS> {
  FormatRequest () = default;

  std::string get_type() const override;

  virtual void exec(PeerDFS* p, message_fun f) override;
};

}
}
