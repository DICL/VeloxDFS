#pragma once
#include "message.hh"
#include "connection.hh"

namespace network {

class Communication {
  public:
    virtual bool connect (std::string, int) = 0;
    virtual bool listen (int)               = 0;
    virtual bool close ()                   = 0;
    virtual bool send (const Message*)      = 0;
    virtual bool recv (Message*)            = 0;

  public:
    Communication() = default;
    virtual ~Communication() { };
};

}
