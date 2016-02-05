#pragma once
#include "../common/context.hh"
#include "fileinfo.hh"
#include <iostream>
#include <fstream>
#include <string>
#include <map>

namespace eclipse {
  class FileIO {
    private:
      Context con;
      FileInfo *f_info;
      std::fstream file;
      std::map<std::string, FileInfo> f_map;

    public:
      FileIO();
      ~FileIO();
      void open_read(int, std::string, bool);  // open file for read
      void open_write(int, std::string, bool);  // open file for write
      void read_file(std::string *);            // read whole file
      void write_file(const std::string *);     // write string
      void read_idata(std::string *);            // read one line
      void write_idata(const std::string *);     // write string
      void close_file();                        // close file
      bool is_end();                            // check eof
  };
}
