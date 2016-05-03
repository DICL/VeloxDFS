#pragma once

#include "executable.hh"
#include "../nodes/fs.hh"
#include <string>

namespace eclipse {
namespace messages {

struct KeyValue: public Executable<FS> {
  KeyValue () = default;
  KeyValue (uint32_t, std::string, std::string);

  std::string get_type() const override;
  uint32_t key;
  std::string name, value;
  void exec(FS*, message_fun) override;
};

}
}
