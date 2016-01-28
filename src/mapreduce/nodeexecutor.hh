#pragma once

namespace eclipse {

class NodeExecutor {
  public:
    NodeExecutor ();
    ~NodeExecutor ();

  protected:
    template <typename T> void process_message (T);

};

} /* eclipse  */ 
