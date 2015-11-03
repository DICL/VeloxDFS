#pragma once

namespace Nodes {
  class iPush {
    public:
      virtual void listen() = 0;
      virtual void accept() = 0;
      virtual void close() = 0;
  };
}
