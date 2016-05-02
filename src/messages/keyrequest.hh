#pragma once

#include "executable.hh"
#include "../nodes/peerdfs.hh"
#include <string>

namespace eclipse {
namespace messages {

struct KeyRequest: public Executable<PeerDFS> {
  KeyRequest () = default;
  KeyRequest (std::string);

  std::string get_type() const override;
  std::string key;
  void exec(PeerDFS*, message_fun) override;
};

}
}
