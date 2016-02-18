#pragma once
#include <fstream>
#include <string>

namespace eclipse {
  class BasicIO {
    private:
      std::fstream file;

    public:
      BasicIO();
      ~BasicIO();
      void open_write(const char*);
      void open_read(const char*);
      void write_file(std::string *);
      void read_file(std::string *);
      void close_file();
  };
}
