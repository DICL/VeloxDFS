#pragma once
#include "../messages/message.hh"

namespace eclipse {

class AsyncNode {
  public:
    virtual ~AsyncNode () { };
    virtual void on_connect() = 0;
    virtual void on_disconnect() = 0;
    virtual void on_read(messages::Message*) = 0;
};

} /* eclipse  */ 
