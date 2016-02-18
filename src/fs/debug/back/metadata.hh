#pragma once
#include "../common/context.hh"
#include <iostream>
#include <string>
#include <sstream>
#include <sqlite3>

namespace eclipse {
  class Metadata {
    private:
      Context con;
      sqlite3 *db;
      char *zErrMsg;
      int rc;
    public:
      Metadata();
      ~Metadata();
      static int callback(void *, int, char **, char **);
      void init();
      void open();
      void exec(const char*);
      void load(std::string);
      void remove(std::string);
      void close();
  };
}
