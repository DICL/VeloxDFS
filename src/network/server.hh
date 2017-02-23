#pragma once
#include "asyncchannel.hh"

namespace eclipse {
namespace network {

class Server: public AsyncChannel {
  public:
    Server(NetObserver*);
    static bool is_multiple();
};

}
}
