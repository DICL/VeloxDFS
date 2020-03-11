#pragma once
#include "router.hh"
#include <memory>

namespace eclipse {

//!
class RouterDecorator: public Router {
  public:
    RouterDecorator(Router*);
    //RouterDecorator(Router*, Router*);
    virtual ~RouterDecorator() = default;

    virtual void on_read(messages::Message*, Channel*);

  protected:
    std::unique_ptr<Router> router;
    //std::unique_ptr<Router> task_router;
};

} /* eclipse  */ 
