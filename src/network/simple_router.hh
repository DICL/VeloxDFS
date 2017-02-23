#pragma once
#include "router.hh"

namespace eclipse {

//
class SimpleRouter: public Router {
  public:
    void on_read(messages::Message*, Channel*) override;
};

} /* eclipse  */ 
