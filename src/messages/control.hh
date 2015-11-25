#pragma once

#include "message.hh"
#include <string>

namespace messages {
  enum {
    SHUTDOWN = 0,
    RESTART  = 1,
    PING     = 2
  };
  struct Control: public Message {
    public:
      Control () = default;
      Control (int);

      std::string get_type() const override;

      int type;
  };

} /* messages */ 
