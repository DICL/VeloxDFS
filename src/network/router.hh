#pragma once
#include "../network/netobserver.hh"
#include <functional>
#include <map>

namespace eclipse {
using namespace eclipse::network;

//! @addtogroup RouterDecorator
//! @{
//! @brief Router \b component entity of Router decorator pattern.
//!
//! Router is intended to be used as a decorator component.
//! The reason of using decorator is to make possible add
//! multiple functionabilities in a flexible way. This is,
//! We might have multiple component which needs to be connected
//! to our network. 
//!
//! Before adding your router, get familiar with decorator pattern.
class Router: public NetObserver {
  public:
    Router() = default;
    virtual ~Router() = default;

    virtual void on_read(messages::Message*, Channel*) = 0;

  protected:
    std::map<std::string, std::function<void(messages::Message*, Channel*)>> routing_table;
};

} /* eclipse  */ 
//! @}
