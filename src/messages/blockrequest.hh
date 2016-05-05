#pragma once

#include "executable.hh"
#include "../nodes/fs.hh"
#include <string>

namespace eclipse {
namespace messages {

struct BlockRequest: public Executable<FS> {
  BlockRequest () = default;

  std::string get_type() const override;
  std::string block_name;
  uint32_t hash_key;
  void exec(FS*, message_fun) override; 
  void on_exec(std::string, std::string, message_fun); 
};

}
}
