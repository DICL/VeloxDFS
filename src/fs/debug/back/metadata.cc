#include "metadata.hh"

using namespace std;

namespace eclipse {

  Metadata::Metadata() { //{{{
    *zErrMsg = 0;
  } //}}}

  Metadata::~Metadata() { //{{{
  } //}}}

  static int Metadata::callback(void *data, int argc, char **argv, char **azColName){ //{{{
    int i;
    con.logger->info("%s: ", (const char*)data);
    for(i=0; i<argc; i++)
    {
      con.logger->info("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    con.logger->info("\n");
    return 0;
  } //}}}

  void Metadata::init() { //{{{
    open();
  }

  bool Metadata::exec(const char* sql) { //{{{
    // Open database
    rc = sqlite3_open("metadata.db", &db);
    if(rc)
    {
      con.logger->error("Can't open metadata: %s\n", sqlite3_errmsg(db));
      return false;
    }
    else
    {
      con.logger->info("Opened metadata successfully\n");
    }

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if(rc != SQLITE_OK)
    {
      con.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
      con.logger->info("Operation done successfully\n");
    }

    // Close SQL database
    sqlite3_close(db);
    return true;
  } //}}}
