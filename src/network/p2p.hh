#pragma once
#include "asyncchannel.hh"

namespace eclipse {
namespace network {

// @pre server and client socket should be open and not null
class P2P: public AsyncChannel {
  public:
    P2P(tcp::socket*, tcp::socket*, NetObserver*, int);
    static bool is_multiple();
};

}
}
