#pragma once
#include "router.hh"
#include <memory>

namespace eclipse {

//!
class RouterDecorator: public Router {
  public:
    RouterDecorator(Router*);
    virtual ~RouterDecorator() = default;

    virtual void on_read(messages::Message*, Channel*);

  protected:
    std::unique_ptr<Router> router;
};

} /* eclipse  */ 
