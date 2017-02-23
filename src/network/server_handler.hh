#pragma once
#include "netobserver.hh"

namespace eclipse {
namespace network {

class ServerHandler {
  public:
    ServerHandler(uint32_t port);
    ~ServerHandler() = default;
    void attach(NetObserver*);

    //! @brief It will start the accepting loop
    bool establish();
    bool close();

  private:
    NetObserver* node;
    uint32_t port;
};

}
} /* eclipse  */ 
