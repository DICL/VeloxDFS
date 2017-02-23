#pragma once

#include "../messages/message.hh"

namespace eclipse {
namespace network {

const int header_size = 16;

class Channel  {
  public:
    Channel () = default;
    virtual ~Channel () = default;

    virtual void do_write (messages::Message*) = 0; 
};

}
}
