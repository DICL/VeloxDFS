#pragma once
//#include "../common/context.hh"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sqlite3>
//#include <sstream>
#include <cstdio>
#include <cstdlib>
#include "blockinfo.h"
#include "fileinfo.h"

namespace eclipse {
  class Directory {
    private:
      //Context con;
      std::fstream file;
      sqlite3 *db;
      char sql[512];
      char *zErrMsg;
      int rc;
      BlockInfo block_info;
      FileInfo file_info;
      streamoff limit;
      void open_write(std::string);  // open file for write
      void open_read(std::string);  // open file for read

    public:
      FileIO();
      ~FileIO();
      void init_db();
      void open_file(std::string, int flag);  // open file for write
      //void open_read(std::string);  // open file for read
      void open_iwrite(int, std::string);  // open file for write
      void open_iread(int, std::string);  // open file for read
      void write_block(const std::string *);     // write string
      void read_block(std::string *);            // read whole file
      void iwrite(const std::string *);     // write string
      void iread(std::string *);            // read one line
      void close_file();                        // close file
      bool is_remain();                            // check eof
      void remove_file(std::string);
      void clear();
  };
}
