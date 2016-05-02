#pragma once

#include "executable.hh"
#include "../nodes/peerdfs.hh"
#include <string>

namespace eclipse {
namespace messages {

struct KeyValue: public Executable<PeerDFS> {
  KeyValue () = default;
  KeyValue (uint32_t, std::string, std::string);

  std::string get_type() const override;
  uint32_t key;
  std::string name, value;
  void exec(PeerDFS*, message_fun) override;
};

}
}
