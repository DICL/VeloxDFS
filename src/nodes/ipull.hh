#pragma once

namespace Nodes {
  class iPull {
    public:
      virtual bool connect () = 0;
      virtual void close () = 0;
      virtual void detach () = 0;
  };
}
