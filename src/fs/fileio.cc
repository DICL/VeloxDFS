#include "fileio.hh"

using namespace std;

namespace eclipse {

  FileIO::FileIO() //{{{ 
  {
    *zErrMsg = 0;
  }
  //}}}

  FileIO::~FileIO() //{{{
  {
    close_file();
  } //}}}

static int FileIO::callback(void *data, int argc, char **argv, char **azColName){ //{{{
  int i;
  con.logger->info("%s: ", (const char*)data);
  for(i=0; i<argc; i++)
  {
    con.logger->info("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }

  con.logger->info("\n");
  return 0;
} //}}}

void FileIO::init_db() //{{{
{
  // Open database
  rc = sqlite3_open("metadata.db", &db);
  if(rc)
  {
    con.logger->error("Can't open metadata: %s\n", sqlite3_errmsg(db));
    return -1;
  }
  else
  {
    con.logger->info("Opened metadata successfully\n");
  }

  // Create SQL statement
  char* sql = "CREATE TABLE METADATA(" \
               "NAME   CHAR(255) PRIMARY KEY NOT NULL," \
               "NETID  INT                   NOT NULL," \
               "TYPE   BOOL                  NOT NULL," \
               "EOF    BOOL                  NOT NULL," \
               "DATAID INT                   );";

  // Execute SQL statement
  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  if(rc != SQLITE_OK)
  {
    con.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  else
  {
    con.logger->info("Table created successfully\n");
  }
  sqlite3_close(db);
} //}}}

void FileIO::open_write(string name) //{{{
{
  // Open database
  rc = sqlite3_open("metadata.db", &db);
  if(rc)
  {
    con.logger->error("Can't open metadata: %s\n", sqlite3_errmsg(db));
    return -1;
  }
  else
  {
    con.logger->info("Opened metadata successfully\n");
  }

  string path = con.settings.get<string>("path.scratch") + "/" + name;
  string stream ss;
  ss << "INSERT INTO METADATA (NAME,NETID,TYPE,EOF) " \
    "VALUES ('";
  ss << path;
  ss << "', ";
  ss << 1;
  ss << ", true, true);";
  string tmp = ss.str();

  // Create SQL statement
  const char* sql = tmp.c_str();

  // Execute SQL statement
  rc = sqlite3_exec(db, sql, callback, rc, &zErrMsg);
  if(rc != SQLITE_OK)
  {
    con.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  else
  {
    file.open(path.c_str(), ios::out | ios::app);
    con.logger->info("Table created successfully\n");
  }
  sqlite3_close(db);
}
/*
   else // intermediate data
   {
   tmp_info.path = con.settings.get<string>("path.idata") + "/" + to_string(jobid) + "_" + name;
   string path = tmp_info.path + "_" + to_string(tmp_info.num_block);
   file.open(path.c_str(), ios::out | ios::app);
   }
   f_map.insert(pair<string, FileInfo>(tmp_info.path, tmp_info));
   auto search = f_map.find(tmp_info.path);
   f_info = &(search->second);

   }

   else // intermediate data
   {
   path = con.settings.get<string>("path.idata") + "/" + to_string(jobid) + "_" + name;
   auto search = f_map.find(path);
   if(search != f_map.end())
   {
 *f_info = search->second;
 path = path + "_" + to_string(f_info->cur_block);
 file.open(path.c_str(), ios::in);
 }
 else
 {
 con.logger->error("[fileIO] Job %d: open_r %s failed.", jobid, name.c_str());
 }
 }
 */
} //}}}
fstream FileIO::open_read(string name) //{{{
{
  // Open database
  rc = sqlite3_open("metadata.db", &db);
  if(rc)
  {
    con.logger->error("Can't open metadata: %s\n", sqlite3_errmsg(db));
    return -1;
  }
  else
  {
    con.logger->info("Opened metadata successfully\n");
  }

  // Create SQL statement

  sql = "SELECT TABLE METADATA(" \
         "NAME   CHAR(255) PRIMARY KEY NOT NULL," \
         "NETID  INT                   NOT NULL," \
         "TYPE   BOOL                  NOT NULL," \
         "DATAID INT                   );";

  // Execute SQL statement
  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  if(rc != SQLITE_OK)
  {
    con.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  else
  {
    con.logger->info("Table created successfully\n");
  }
  sqlite3_close(db);
  string path = con.settings.get<string>("path.scratch") + "/" + name;
  BlockInfo block_info;
  block_info.path = con.settings.get<string>("path.scratch") + "/" + name;
  block_info.data_id = -1;
  block_info.type = true;
  FileInfo file_info;
  file_info.block_info.push(block_info);
  file_map.push(file_info);
  file.open(path.c_str(), ios::in);
} //}}}
void FileIO::open_write(int jobid, string name, bool type) //{{{
{ 
} //}}}
void FileIO::read_file(string *buf) //{{{
{
  buf->assign((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));
}

void FileIO::read_idata(string *buf)
{
  getline(file, *buf);
  if(file.eof())
  {
    f_info->cur_block++;
    if(f_info->cur_block < f_info->num_block)
    {
      file.close();
      string path = f_info->path + "_" + to_string(f_info->cur_block + 1);
      file.open(path.c_str(), ios::in | ios::app);
    }
  }
} //}}}
void FileIO::write_file(const string *buf) //{{{
{
  file << *buf;
}//}}} 
void FileIO::write_idata(const string *buf)//{{{
{
  size_t limit = con.settings.get<int>("block.size");
  // con.logger->info("size of size = %d", f_info->size);
  //string tmp = *buf;
  //con.logger->info("size of tmp = %d", tmp.length());
  //con.logger->info("tmp = %s", tmp.c_str());


  if(f_info->size + buf->length() > limit)
  {
    //con.logger->info("OVER!!!!");
    file.close();
    string path = f_info->path + "_" + to_string(++(f_info->num_block));
    file.open(path.c_str(), ios::out | ios::app);
    f_info->size = 0;
  }      
  file << *buf;
  f_info->size += buf->length();
  //con.logger->info("size of tmp = %d", tmp.length());
  //con.logger->info("size of size = %d", f_info->size);
}//}}}
void FileIO::close_file() //{{{
{
  if(file.is_open())
  {
    file.close();
  }
} //}}

bool FileIO::is_end() //{{{
{
  if(f_info->cur_block < f_info->num_block)
  {
    return false;
  }
  else
  {
    return true;
  }
}//}}}
}
