#pragma once
#include "../messages/message.hh"

namespace eclipse {
namespace network {

class Network {
  public:
    virtual ~Network () { }

    virtual bool establish() = 0;
    virtual bool close () = 0;
    virtual size_t size () = 0;
    virtual bool send(int, messages::Message*) = 0;
};

}
} /* eclipse  */ 
