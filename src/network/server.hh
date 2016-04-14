#pragma once
#include "asyncchannel.hh"

namespace eclipse {
namespace network {

class Server: public AsyncChannel {
  public:
    Server(tcp::socket*,tcp::socket*, NetObserver*, int);
    static bool is_multiple();
};

}
}
