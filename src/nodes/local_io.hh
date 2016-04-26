#pragma once
#include <string>

namespace eclipse {

class Local_io {
  public:
    void write (std::string, std::string);
    std::string read (std::string);
    void remove (std::string);
    bool format ();

    Local_io();
   
  private:
    std::string disk_path;
};

}
