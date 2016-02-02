#pragma once

namespace eclipse {

class Executor: MR_traits {
  public:
    Executor ();
    ~Executor ();

    bool load(std::string) override;
  protected:
    template <typename T> void process_message (T);
};

} /* eclipse  */ 
