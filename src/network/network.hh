#pragma once
#include "../messages/message.hh"
#include "asyncnode.hh"
#include <vector>

namespace eclipse {
namespace network {

class Network {
  public:
    virtual ~Network () { }

    virtual bool establish() = 0;
    virtual bool close () = 0;
    virtual size_t size () = 0;
    virtual bool send(int, messages::Message*) = 0;
    virtual bool send_and_replicate(std::vector<int>, messages::Message*) = 0;
    virtual void attach (AsyncNode*) = 0;
};

}
} /* eclipse  */ 
