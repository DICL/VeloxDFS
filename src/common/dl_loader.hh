#pragma once

#include <string>
#include <utility>

typedef std::pair<std::string, std::string>(*maptype)(std::string);
class DL_loader {
  public:
    DL_loader(std::string);
    ~DL_loader();

    bool init_lib ();
    maptype load_function (std::string);
    void close();


  protected:
    std::string lib_name;
    void* lib= nullptr;
};
