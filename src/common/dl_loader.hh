#pragma once

#include <string>

class DL_loader {
  public:
    DL_loader(std::string, std::string);
    ~DL_loader();

    void run();

  protected:
    void (*func_) () = nullptr;
    void* lib  = nullptr;
};
