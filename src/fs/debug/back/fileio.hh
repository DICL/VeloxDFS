#pragma once
#include "../common/context.hh"
#include "fileinfo.hh"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sqlite3>
#include <sstream>

namespace eclipse {
  class FileIO {
    private:
      Context con;
      std::fstream file;
      sqlite3 *db;
      char *zErrMsg;
      int rc;
      vector<BlockInfo> block_info;

    public:
      FileIO();
      ~FileIO();
      void init_db();
      void open_write(std::string);  // open file for write
      void open_read(std::string);  // open file for read
      void open_iwrite(int, std::string);  // open file for write
      void open_iread(int, std::string);  // open file for read
      void write_file(const std::string *);     // write string
      void read_file(std::string *);            // read whole file
      void write_idata(const std::string *);     // write string
      void read_idata(std::string *);            // read one line
      void close_file();                        // close file
      bool is_remain();                            // check eof
      void remove_file(std::string);
      void clear();
  };
}
