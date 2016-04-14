#pragma once
#include "../messages/message.hh"

namespace eclipse {

class AsyncNode {
  public:
    virtual ~AsyncNode () { };
    virtual void on_connect() = 0;
    virtual void on_disconnect(int) = 0;
    virtual void on_read(messages::Message*, int) = 0;
};

} /* eclipse  */ 
