#pragma once

#include "executable.hh"
#include "../nodes/peerdfs.hh"
#include <string>

namespace eclipse {
namespace messages {

enum {
  SHUTDOWN = 0,
  RESTART  = 1,
  PING     = 2
};
struct Control: public Executable<PeerDFS> {
  public:
    Control () = default;
    Control (int);

    std::string get_type() const override;

    int type;

    void exec(PeerDFS* p, message_fun) override { }
};

} /* messages */ 
}
