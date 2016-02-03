#pragma once

#include <string>
#include "../mapreduce/PeerLocalMR.hh"

class DL_loader {
  typedef void (*maptype)();
  public:
    DL_loader(std::string, std::string);
    ~DL_loader();

    bool init_executor (PeerLocalMR*);
    maptype load_function (std::string);


  protected:
    std::string input, lib_name
    void* lib_handler = nullptr;
};
