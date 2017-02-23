#pragma once
#include "../network/router_decorator.hh"
#include "block_node.hh"

namespace eclipse {

class BlockNodeRouter: public RouterDecorator {
  public:
    BlockNodeRouter(BlockNode*, Router*);
    ~BlockNodeRouter() = default;

    void io_operation(messages::Message*, Channel*);

  private:
    BlockNode* block_node = nullptr;
};

} /* eclipse  */ 
