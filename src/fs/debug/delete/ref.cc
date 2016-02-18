#include "fileio.hh"
using namespace std;

namespace eclipse {
  Directory::Directory() //{{{ 
  {
    *zErrMsg = 0;
    block_counter = 0;
  }
  //}}}
  Directory::~Directory() //{{{
  {
    close_file();
    sqlite3_close(db);
  } //}}}
static int Directory::init_callback(void *NotUsed, int argc, char **argv, char **azColName){ //{{{
  for(int i=0; i<argc; i++){
    //con.logger->info("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  //con.logger->info("\n");
  printf("\n");
  return 0;
} //}}}
static int Directory::file_callback(void *NotUsed, int argc, char **argv, char **azColName){ //{{{
  for(int i=0; i<argc; i++)
  {
    file_info.file_id = argv[i];
    file_info.file_name = argv[i];
    file_info.file_size = argv[i];
    file_info.num_block = argv[i];
    file_info.replica = argv[i];
  }
} //}}}
static int Directory::block_callback(void *NotUsed, int argc, char **argv, char **azColName){ //{{{
  for(int i=0; i<argc; i++)
  {
    block_info.file_id = argv[i];
    block_info.block_seq = argv[i];
    block_info.block_name = argv[i];
    block_info.real_name = argv[i];
    block_info.block_size = argv[i];
    block_info.is_inter = argv[i];
    block_info.node = argv[i];
    block_info.l_node = argv[i] ? argv[i] : "NULL";
    block_info.r_node = argv[i] ? argv[i] : "NULL";
    block_info.commit = argv[i] ? argv[i] : false;
  }
} //}}}
static int Directory::exist_callback(void *result, int argc, char **argv, char **azColName){ //{{{
  *(bool *result) = argv[i]? true : false;
} //}}}
bool Directory::init_db() //{{{
{
  // Open database
  rc = sqlite3_open("metadata.db", &db);
  if(rc)
  {
    //con.logger->error("Can't open metadata: %s\n", sqlite3_errmsg(db));
    printf("Can't open metadata: %s\n", sqlite3_errmsg(db));
    return false;
  }
  else
  {
    //con.logger->info("Opened metadata successfully\n");
    printf("Opened metadata successfully\n");
  }

  // Create SQL statement
  sprintf(sql, "CREATE TABLE file_table(" \
      "file_id         INT   NOT NULL," \
      "file_name       TEXT  NOT NULL," \
      "file_hash_key   INT   NOT NULL," \
      "file_size       INT   NOT NULL," \
      "num_block       INT   NOT NULL," \
      "replica         INT   NOT NULL," \
      "PRIMARY KEY (file_id));"; 

      // Execute SQL statement
      rc = sqlite3_exec(db, sql, init_callback, 0, &zErrMsg);
      if(rc != SQLITE_OK)
      {
      //con.logger->error("SQL error: %s\n", zErrMsg);
      printf("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      }
      else
      {
      //con.logger->info("Table created successfully\n");
      printf("Table created successfully\n");
      }
      // Create SQL statement
      sprintf(sql, "CREATE TABLE block_table(" \
          "file_id        INT       NOT NULL," \
          "block_seq      INT       NOT NULL," \
          "block_hash_key INT       NOT NULL," \
          "block_name     TEXT      NOT NULL," \
          "block_size     INT       NOT NULL," \
          "is_inter       BOOLEAN   NOT NULL," \
          "node           TEXT      NOT NULL," \
          "l_node         TEXT              ," \
          "r_node         TEXT              ," \
          "commit         BOOLEAN           ," \
          "PRIMARY KEY (file_id, block_seq));"; 

//          "real_name   TEST      NOT NULL," \

          // Execute SQL statement
          rc = sqlite3_exec(db, sql, init_callback, 0, &zErrMsg);
          if(rc != SQLITE_OK)
          {
          //con.logger->error("SQL error: %s\n", zErrMsg);
          printf("SQL error: %s\n", zErrMsg);
          sqlite3_free(zErrMsg);
          }
          else
          {
            //con.logger->info("Table created successfully\n");
            printf("Table created successfully\n");
          }
          sqlite3_close(db);
          return true;
} //}}}
bool Directory::is_exist(uint32_t file_id) //{{{
{
  bool *result = new bool;
  // Create SQL statement
  sprintf(sql, "SELECT * from file_table where file_id=%u;");

  // Execute SQL statement
  rc = sqlite3_exec(db, sql, file_callback, result, &zErrMsg);
  if(rc != SQLITE_OK)
  {
    //con.logger->error("SQL error: %s\n", zErrMsg);
    printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  else
  {
    //con.logger->info("Table created successfully\n");
    printf("Table selected successfully\n");
  }
  bool return_bool = *result;
  delete result;
  return return_bool;
} //}}}

bool Directory::open_file(string file_name, int flag) //{{{
  int file_id=0;

//  try {
      if(flag==O_WRONLY){
        file_id = open_write_file(file_name);
      }
      else if(flag==O_RDONLY){
        file_id = open_read_file(file_name);
      }
//  }
//  catch(){
//  }
}

int Directory::open_write_file(string file_name) //{{{
{
  // Open database
  rc = sqlite3_open("metadata.db", &db);
  if(rc)
  {
    //con.logger->error("Can't open metadata: %s\n", sqlite3_errmsg(db));
    printf("Can't open metadata: %s\n", sqlite3_errmsg(db));
    return false;
  }
  else
  {
    //con.logger->info("Opened metadata successfully\n");
    printf("Opened metadata successfully\n");
  }

  // Open file
  //file.open(path.c_str(), ios::out | ios::app);

  // Create file info
  do
  {
    file_info.file_id = rand();
  }
  while(is_exist(file_info.file_id));  // generate a unique file id
  file_info.file_name = con.settings.get<string>("path.scratch") + "/" + file_name;
  file_info.file_size = 0;
  file_info.num_block = 0;
  file_info.replica = con.settings.get<int>("block.replica");

  // Create SQL statement
  sprintf(sql, "INSERT INTO file_table (file_id, file_name, file_size, num_block, replica) VALUES (%u, %s, %u, %u, %u);", file_info.file_id, file_info.file_name, file_info.file_size, file_info.num_block, file_info.replica);

  // Execute SQL statement
  rc = sqlite3_exec(db, sql, file_callback, result, &zErrMsg);
  if(rc != SQLITE_OK)
  {
    //con.logger->error("SQL error: %s\n", zErrMsg);
    printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  else
  {
    //con.logger->info("Table created successfully\n");
    printf("File metadata is inserted into file_table successfully\n");
  }
  /*
  // Create block info
  block_info.file_id = file_info.file_id;
  block_info.block_seq = 0; // start from 0
  block_info.block_name = file_info.file_name + "_" + block_info.block_seq;
  block_info.real_name = argv[i];
  block_info.block_size = argv[i];
  block_info.is_inter = argv[i];
  block_info.node = argv[i];
  block_info.l_node = argv[i] ? argv[i] : "NULL";
  block_info.r_node = argv[i] ? argv[i] : "NULL";
  block_info.commit = argv[i] ? argv[i] : false;
  */

  return file_info.file_id;
} //}}}

bool Directory::open_read_file(string file_name) //{{{
{
  // Open database
  rc = sqlite3_open("metadata.db", &db);
  if(rc)
  {
    //con.logger->error("Can't open metadata: %s\n", sqlite3_errmsg(db));
    printf("Can't open metadata: %s\n", sqlite3_errmsg(db));
    return false;
  }
  else
  {
    //con.logger->info("Opened metadata successfully\n");
    printf("Opened metadata successfully\n");
  }

  // Create SQL statement
  stringstream ss;
  ss << "SELECT * from METADATE where file_name='";
  ss << file_name;
  ss << "';";
  string tmp_str = ss.str();
  const char* sql = tmp.c_str();

  // Execute SQL statement
  rc = sqlite3_exec(db, sql, read_callback, rc, &zErrMsg);
  if(rc != SQLITE_OK)
  {
    //con.logger->error("SQL error: %s\n", zErrMsg);
    printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    return false;
  }
  else
  {
    //file_info.insert(pair<string, BlockInfo>(block_info.file_name, block_info));
    //con.logger->info("Table created successfully\n");
    printf("Table selected successfully\n");
  }

  // Open file
  block_info = file_info.pop();
  string path = block_info.path + "/" + block_info.block_name;
  file.open(path.c_str(), ios::in);
  return true;
} //}}}

bool Directory::insert_block_metadata(const BlockInfo block_info) //{{{
{
  block_info.eof = true;
  // con.logger->info("size of size = %d", f_info->size);
  //string tmp = *buf;
  //con.logger->info("size of tmp = %d", tmp.length());
  //con.logger->info("tmp = %s", tmp.c_str());
  if(block_info.block_size + buf->length() > limit)
  {
    //con.logger->info("OVER!!!!");
    file.close();
    block_info.eof = false;

    // Create sql statement
    string stream ss;
    ss << "INSERT INTO METADATA (" \
      "file_name," \
      "file_size," \
      "block_name," \
      "block_size," \
      "num_block," \
      "block_order," \
      "eof," \
      "ip_address," \
      "path) " \
      "VALUES ('";
    ss << block_info.file_name;
    ss << "', ";
    ss << block_info.file_size;
    ss << ", '";
    ss << block_info.block_name;
    ss << "', ";
    ss << block_info.block_size;
    ss << ", ";
    ss << block_info.num_block;
    ss << ", ";
    ss << block_info.block_order;
    ss << ", ";
    ss << block_info.eof;
    ss << ", '";
    ss << block_info.ip_address;
    ss << "', '";
    ss << block_info.path;
    ss << "');";
    string tmp_str = ss.str();
    const char* sql = tmp.c_str();

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, callback, rc, &zErrMsg);
    if(rc != SQLITE_OK)
    {
      //con.logger->error("SQL error: %s\n", zErrMsg);
      printf("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return false;
    }
    else
    {
      //file_info.insert(pair<string, BlockInfo>(block_info.file_name, block_info));
      //con.logger->info("Table created successfully\n");
      printf("Table created successfully\n");
    }

    // Open new block
    block_info.block_name = block_info.file_name + "_" + to_string(++(block_info.order));
    // TODO: if hash(block_name) != local, send to peer node
    string path = block_info.path + "/" + block_info.block_name;
    //file.open(path.c_str(), ios::out | ios::app);
    block_info.file_size += block_info.block_size;
    block_info.block_size = 0;
  }
  //file << *buf;
  block_info.block_size += buf->length();
  return true;
  //con.logger->info("size of tmp = %d", tmp.length());
  //con.logger->info("size of size = %d", f_info->size);
  // Create SQL statement
} //}}} 
void Directory::read_file(string *buf) //{{{
{
  buf->assign((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));
}

void Directory::read_idata(string *buf)
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
void Directory::write_idata(const string *buf)//{{{
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
void Directory::close_file() //{{{
{
  if(file.is_open())
  {
    file.close();
  }
} //}}

bool Directory::is_end() //{{{
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

// REMAIN THINGS
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
fstream Directory::open_read(string name) //{{{
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
void Directory::open_write(int jobid, string name, bool type) //{{{
{ 
} //}}}
