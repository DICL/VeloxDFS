#pragma once

#include "channel.hh"
#include "asyncnode.hh"
#include "../messages/message.hh"
#include <string>

namespace eclipse {
namespace network {

class AsyncChannel: public Channel {
  public:
    AsyncChannel(Context&, int, AsyncNode*);
    virtual void do_connect () = 0;
    virtual void do_write (messages::Message*) = 0; 

  protected:
    AsyncNode* node = nullptr;
    std::string host;
};

}
}
