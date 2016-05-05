#pragma once

#include "executable.hh"
#include "../nodes/fs.hh"
#include <string>

namespace eclipse {
namespace messages {

enum {
  SHUTDOWN = 0,
  RESTART  = 1,
  PING     = 2
};
struct Control: public Executable<FS> {
  public:
    Control () = default;
    Control (int);

    std::string get_type() const override;

    int type;

    void exec(FS* p, message_fun) override { }
};

} /* messages */ 
}
