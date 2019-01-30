#include "directory.hh"
#include "../common/context_singleton.hh"
#include <sqlite3.h>
#include <cinttypes>

#define DEFAULT_QUERY_SIZE 2048

using namespace std;
using namespace eclipse;

// Callback and free functions {{{
static int file_callback(void *file_info, int argc, char **argv, char **azColName) {
  int i = 0;
  auto file = reinterpret_cast<FileInfo*>(file_info);
  file->name          = argv[i++];
  file->hash_key      = atoi(argv[i++]);
  file->size          = atoll(argv[i++]);
  file->num_block     = atoi(argv[i++]);
  file->n_lblock      = atoi(argv[i++]);
  file->type          = atoi(argv[i++]);
  file->replica       = atoi(argv[i++]);
  file->uploading     = atoi(argv[i++]);
  file->is_input      = atoi(argv[i++]);
  file->intended_block_size = atoi(argv[i]);
  return 0;
}

static int block_callback(void *block_info, int argc, char **argv, char **azColName) {
  int i = 0;
  auto block = reinterpret_cast<BlockMetadata*>(block_info);
  block->name         = argv[i++];
  block->file_name    = argv[i++];
  block->seq          = atoi(argv[i++]);
  block->hash_key     = atoi(argv[i++]);
  block->size         = atoi(argv[i++]);
  block->type         = atoi(argv[i++]);
  block->replica      = atoi(argv[i++]);
  block->node         = argv[i++];
  block->l_node       = argv[i] ? argv[i] : "NULL";
  i++;
  block->r_node       = argv[i] ? argv[i] : "NULL";
  i++;
  block->is_committed = argv[i] ? atoi(argv[i]) : 0;
  return 0;
}


static int file_list_callback(void *list, int argc, char **argv, char **azColName) {
  auto file_list = reinterpret_cast<vector<FileInfo>*>(list);
  for (int i=0; i<argc; i++) {
    FileInfo tmp_file;
    tmp_file.name      = argv[i++];
    tmp_file.hash_key  = atoi(argv[i++]);
    tmp_file.size      = atoll(argv[i++]);
    tmp_file.num_block = atoi(argv[i++]);
    tmp_file.n_lblock  = atoi(argv[i++]);
    tmp_file.type      = atoi(argv[i++]);
    tmp_file.replica   = atoi(argv[i++]);
    tmp_file.uploading = atoi(argv[i++]);
    tmp_file.is_input  = atoi(argv[i++]);
    tmp_file.intended_block_size = atoi(argv[i]);
    file_list->push_back(tmp_file);
  }
  return 0;
}

static int block_list_callback(void *list, int argc, char **argv, char **azColName) {
  auto block_list = reinterpret_cast<vector<BlockMetadata>*>(list);
  for (int i=0; i<argc; i++) {
    BlockMetadata tmp_block;
    tmp_block.name      = argv[i++];
    tmp_block.file_name = atoi(argv[i++]);
    tmp_block.seq       = atoi(argv[i++]);
    tmp_block.hash_key  = atoi(argv[i++]);
    tmp_block.size      = atoi(argv[i++]);
    tmp_block.type      = atoi(argv[i++]);
    tmp_block.replica   = atoi(argv[i++]);
    tmp_block.node      = argv[i++];
    tmp_block.l_node    = argv[i] ? argv[i] : "NULL";
    i++;
    tmp_block.r_node    = argv[i] ? argv[i] : "NULL";
    i++;
    tmp_block.is_committed    = argv[i] ? atoi(argv[i]) : 0;
    block_list->push_back(tmp_block);
  }
  return 0;
}

static int exist_callback(void *result, int argc, char **argv, char **azColName) {
  *reinterpret_cast<bool*>(result) = argv[0] ? true : false;
  return 0;
}
// }}}
// open {{{
static sqlite3* open(string path) {
  sqlite3* db = NULL;

  int rc;
  if ((rc = sqlite3_open_v2(path.c_str(), &db,
          SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL)) != SQLITE_OK) {
    ERROR("Can't open database: %i", rc);
  } else {
    DEBUG("Opened database successfully");
  }
  return db;
}
// }}}

// contructor {{{
Directory::Directory() {
  path = GET_STR("path.metadata") + "/metadata.db";
}
// }}}
// query_exec_simple {{{
bool Directory::query_exec_simple(char* query, int (*fn)(void*,int,char**,char**) = NULL, void* argv = NULL) {
  char *zErrMsg = nullptr;
  sqlite3* db = nullptr;

  int rc = SQLITE_OK;
  do {
    db = open(path);
    rc = sqlite3_exec(db, query, fn, argv, &zErrMsg);
    if (rc != SQLITE_OK) {
      ERROR("SQL error: %s error_code=%d", zErrMsg, rc);
      sqlite3_free(zErrMsg);
      zErrMsg = nullptr;
      if (rc == SQLITE_LOCKED or rc == SQLITE_BUSY) {
        INFO("SQLITE locked, retrying...");
        sleep(1); // Try again
      }
      else 
        break;
    }
  } while (SQLITE_OK != rc);

  sqlite3_close(db);

  return rc;
}
// }}}
// create_tables {{{
void Directory::create_tables() {
  char sql[DEFAULT_QUERY_SIZE];

  sprintf(sql, "CREATE TABLE IF NOT EXISTS file_table( \
      name       TEXT  NOT NULL, \
      hash_key   INT   NOT NULL, \
      size       INT   NOT NULL, \
      num_block  INT   NOT NULL, \
      n_lblock   INT   NOT NULL, \
      type       INT   NOT NULL, \
      replica    INT   NOT NULL, \
      uploading  INT   NOT NULL, \
      is_input   INT   NOT NULL, \
      intended_block_size INT   NOT NULL, \
      PRIMARY KEY (name));"); 

  if (query_exec_simple(sql))
    DEBUG("file_table created successfully");

  sprintf(sql, "CREATE TABLE IF NOT EXISTS block_table( \
      name          TEXT      NOT NULL, \
      file_name     TEXT      NOT NULL, \
      seq           INT       NOT NULL, \
      hash_key      INT       NOT NULL, \
      size          INT       NOT NULL, \
      type          INT       NOT NULL, \
      replica       INT       NOT NULL, \
      node          TEXT      NOT NULL, \
      l_node        TEXT              , \
      r_node        TEXT              , \
      is_committed  INT               , \
      PRIMARY KEY (name));"); 

  if (query_exec_simple(sql))
    DEBUG("block_table created successfully");
}
// }}}

// file_table_insert {{{
void Directory::file_table_insert (FileInfo &file_info) {
  char sql[DEFAULT_QUERY_SIZE];
  
  sprintf(sql, "INSERT INTO file_table (\
    name, hash_key, size, num_block, n_lblock, type, replica, uploading, is_input, intended_block_size)\
    VALUES('%s', %" PRIu32 ", %" PRIu64 ", %u, %u, %u, %u, %u, %u, %" PRIu64 ");",
      file_info.name.c_str(),
      file_info.hash_key,
      file_info.size,
      file_info.num_block,
      file_info.n_lblock,
      file_info.type,
      file_info.replica,
      file_info.uploading,
      file_info.is_input,
      file_info.intended_block_size);

  if (query_exec_simple(sql))
    DEBUG("file_metadata inserted successfully");
}
// }}}
// file_table_select {{{
void Directory::file_table_select(string name, FileInfo *file_info) {
  char sql[DEFAULT_QUERY_SIZE];

  sprintf(sql, "SELECT * from file_table where name='%s';", name.c_str());
  query_exec_simple(sql, file_callback, (void*)file_info);
}
// }}}
// file_table_select_all {{{
void Directory::file_table_select_all(vector<FileInfo> &file_list) {
  char sql[DEFAULT_QUERY_SIZE];

  sprintf(sql, "SELECT * from file_table where uploading=0;");
  query_exec_simple(sql, file_list_callback, (void*)&file_list);
}
// }}}
// file_table_update {{{
void Directory::file_table_update(string file_name, uint64_t size, uint32_t num_block) {
  char sql[DEFAULT_QUERY_SIZE];

  sprintf(sql, "UPDATE file_table set \
      size=%" PRIu64 ", num_block=%u where name='%s';",
      size, num_block, file_name.c_str());

  if (query_exec_simple(sql))
    DEBUG("file_metadata updated successfully");

}
// }}}
// file_table_delete {{{
void Directory::file_table_delete(string name) {
  char sql[DEFAULT_QUERY_SIZE];

  sprintf(sql, "DELETE from file_table where name='%s';", name.c_str());
  if (query_exec_simple(sql))
    DEBUG("file_metadata deleted successfully");
}
// }}}
// file_table_exists {{{
bool Directory::file_table_exists(string name) {
  char sql[DEFAULT_QUERY_SIZE];
  bool result = false;

  sprintf(sql, "SELECT name from file_table where name='%s';", name.c_str());
  if (query_exec_simple(sql, exist_callback, &result))
    DEBUG("file_exist executed successfully");

  return result;
}
// }}}
// file_table_confirm_upload  {{{
void Directory::file_table_confirm_upload (std::string file_name, uint32_t num_block) {
  char sql[DEFAULT_QUERY_SIZE];

  sprintf(sql, "UPDATE file_table set uploading=0, num_block=%u where name='%s';", num_block,
      file_name.c_str());
  query_exec_simple(sql);
}
// }}}

// block_table_insert {{{
void Directory::block_table_insert(BlockMetadata& metadata) {
  char sql[DEFAULT_QUERY_SIZE];

  sprintf(sql, "INSERT OR REPLACE INTO block_table (\
    name, file_name, seq, hash_key, size, type, replica, node, l_node, r_node, is_committed)\
    VALUES ('%s', '%s', %u, %" PRIu32 ", %" PRIu32 ", %u, %u, '%s', '%s', '%s', %u);",
      metadata.name.c_str(),
      metadata.file_name.c_str(),
      metadata.seq,
      metadata.hash_key,
      metadata.size,
      metadata.type,
      metadata.replica,
      metadata.node.c_str(),
      metadata.l_node.c_str(),
      metadata.r_node.c_str(),
      metadata.is_committed);

  if (query_exec_simple(sql))
    DEBUG("block_metadata inserted successfully");
}
// }}}
// block_table_insert_all {{{
void Directory::block_table_insert_all(std::vector<BlockMetadata>& metadata) {
  char *zErrMsg = nullptr;
  sqlite3* db = nullptr;
  sqlite3_stmt * stmt;
  const char * tail = 0;
  db = open(path);

  char sql[DEFAULT_QUERY_SIZE];
  sprintf(sql, "INSERT OR REPLACE INTO block_table VALUES (@NAME, @FNAME, @SEQ, @H , @SZ , @TYPE, @REP, @NODE, @LNODE, @RNODE, @COM);");
  sqlite3_prepare_v2(db,  sql, DEFAULT_QUERY_SIZE, &stmt, &tail);

  sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &zErrMsg);

  for (auto& block : metadata) {
    sqlite3_bind_text  (stmt, 1, block.name.c_str()     , -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 2, block.file_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int   (stmt, 3, block.seq);
    sqlite3_bind_int64 (stmt, 4, block.hash_key);
    sqlite3_bind_int64 (stmt, 5, block.size);
    sqlite3_bind_int   (stmt, 6, block.type);
    sqlite3_bind_int   (stmt, 7, block.replica);
    sqlite3_bind_text  (stmt, 8, block.node.c_str()     , -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 9, block.l_node.c_str()   , -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt,10, block.r_node.c_str()   , -1, SQLITE_TRANSIENT);
    sqlite3_bind_int   (stmt,11, block.is_committed);

    sqlite3_step(stmt);        

    sqlite3_clear_bindings(stmt);
    sqlite3_reset(stmt);
  }

  sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &zErrMsg);

  sqlite3_finalize(stmt);
  sqlite3_close(db);
}
// }}}
// block_table_select {{{
void Directory::block_table_select(string file_name, std::vector<BlockMetadata>& blocks) {
  char sql[DEFAULT_QUERY_SIZE];

  sprintf(sql, "SELECT * from block_table where (file_name='%s');", file_name.c_str());

  query_exec_simple(sql, block_list_callback, (void*)&blocks);
} 
// }}}
// block_table_select_by_index {{{
void Directory::block_table_select_by_index(string file_name, unsigned int block_seq, BlockMetadata *block_info) {
  char sql[DEFAULT_QUERY_SIZE];

  sprintf(sql, "SELECT * from block_table where (file_name='%s') and \
      (seq=%u);", file_name.c_str(), block_seq);

  query_exec_simple(sql, block_callback, (void*)block_info);
} 
// }}}
// block_table_select_all {{{
void Directory::block_table_select_all(vector<BlockMetadata> &block_info) {
  char sql[DEFAULT_QUERY_SIZE];

  sprintf(sql, "SELECT * from block_table;");
  query_exec_simple(sql, block_list_callback, (void*)&block_info);
} 
// }}}
// block_table_update {{{
void Directory::block_table_update(string file_name, uint32_t size, uint32_t seq) {
  char sql[DEFAULT_QUERY_SIZE];

  sprintf(sql, "UPDATE block_table set \
      size=%" PRIu32 " where (file_name='%s') and (seq=%u);",
      size, file_name.c_str(), seq);

  if (query_exec_simple(sql))
    DEBUG("block_metadata updated successfully");
}
// }}}
// block_table_delete {{{
void Directory::block_table_delete(string file_name, unsigned int seq) {
  char sql[DEFAULT_QUERY_SIZE];

  sprintf(sql, "DELETE from block_table where (file_name='%s') and (seq=%u);", file_name.c_str(), seq);
  if (query_exec_simple(sql))
    DEBUG("block_metadata deleted successfully");
}
// }}}
// block_table_delete_all {{{
void Directory::block_table_delete_all(string file_name) {
  char sql[DEFAULT_QUERY_SIZE];

  sprintf(sql, "DELETE from block_table where (file_name='%s');", file_name.c_str());
  if (query_exec_simple(sql))
    DEBUG("block_metadata deleted successfully");
}
// }}}
// select_last_block_metadata {{{
void Directory::select_last_block_metadata(string file_name, 
    BlockMetadata *block_info) {
  char sql[DEFAULT_QUERY_SIZE];

  sprintf(sql, "SELECT * FROM block_table WHERE (file_name='%s') \
      ORDER BY seq DESC LIMIT 1;", file_name.c_str());

  if (query_exec_simple(sql, block_callback, (void*)block_info))
    DEBUG("The last block_metadata selected successfully");
}
// }}}
