#include "directory.hh"
using namespace std;

namespace eclipse{
  Directory::Directory()
  {
  }

  Directory::~Directory()
  {
  }

  int Directory::file_callback(void *file_info, int argc, char **argv, char **azColName)
  {
    int i = 0;
    auto file = reinterpret_cast<FileInfo*>(file_info);
    file->file_id       = atoi(argv[i++]);
    file->file_name     = argv[i++];
    file->file_hash_key = atoi(argv[i++]);
    file->file_size     = atoi(argv[i++]);
    file->num_block     = atoi(argv[i++]);
    file->replica       = atoi(argv[i]);
    return 0;
  }

  int Directory::block_callback(void *block_info, int argc, char **argv, char **azColName)
  {
    int i = 0;
    auto block = reinterpret_cast<BlockInfo*>(block_info);
    block->file_id        = atoi(argv[i++]);
    block->block_seq      = atoi(argv[i++]);
    block->block_hash_key = atoi(argv[i++]);
    block->block_name     = argv[i++];
    block->block_size     = atoi(argv[i++]);
    block->is_inter       = atoi(argv[i++]);
    block->node           = argv[i++];
    block->l_node = argv[i] ? argv[i] : "NULL";
    i++;
    block->r_node = argv[i] ? argv[i] : "NULL";
    i++;
    block->is_commit = argv[i] ? atoi(argv[i]) : 0;
    return 0;
  } 

  int Directory::display_callback(void *data, int argc, char **argv, char **azColName){
    Context con;
    con.logger->info("%s: ", (const char*)data);
    for(int i=0; i<argc; i++){
      con.logger->info("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
  }

  int Directory::exist_callback(void *result, int argc, char **argv, char **azColName){
    *reinterpret_cast<bool*>(result) = argv[0] ? true : false;
    return 0;
  }

  void Directory::init_db()
  {
    // Open database
    sqlite3 *db;
    char *zErrMsg = 0;
    string path = con.settings.get<string>("path.metadata") + "/metadata.db";
    int rc = sqlite3_open(path.c_str(), &db);
    char sql[512];
    if(rc)
    {
      con.logger->error("Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
      con.logger->info("Opened database successfully\n");
    }
    // Create SQL statement
    sprintf(sql, "CREATE TABLE file_table( \
      file_id         INT   NOT NULL, \
        file_name       TEXT  NOT NULL, \
        file_hash_key   INT   NOT NULL, \
        file_size       INT   NOT NULL, \
        num_block       INT   NOT NULL, \
        replica         INT   NOT NULL, \
        PRIMARY KEY (file_id));"); 

        // Execute SQL statement
        rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if(rc != SQLITE_OK)
    {
      con.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
      con.logger->info("file_table created successfully\n");
    }
    // Create SQL statement
    sprintf(sql, "CREATE TABLE block_table( \
      file_id        INT       NOT NULL, \
        block_seq      INT       NOT NULL, \
        block_hash_key INT       NOT NULL, \
        block_name     TEXT      NOT NULL, \
        block_size     INT       NOT NULL, \
        is_inter       INT       NOT NULL, \
        node           TEXT      NOT NULL, \
        l_node         TEXT              , \
        r_node         TEXT              , \
        is_commit      INT               , \
        PRIMARY KEY (file_id, block_seq));"); 

        // Execute SQL statement
        rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if(rc != SQLITE_OK)
    {
      con.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
      con.logger->info("block_table created successfully\n");
    }
    // Close Database
    sqlite3_close(db);
  }

  void Directory::insert_file_metadata(FileInfo file_info)
  {
    // Open database
    sqlite3 *db;
    char sql[512];
    char *zErrMsg = 0;
    string path = con.settings.get<string>("path.metadata") + "/metadata.db";
    int rc = sqlite3_open(path.c_str(), &db);
    if(rc)
    {
      con.logger->error("Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
      con.logger->info("Opened database successfully\n");
    }

    // Create sql statement
    sprintf(sql, "INSERT INTO file_table (\
      file_id, file_name, file_hash_key, file_size, \
        num_block, replica) \
      VALUES (%" PRIu32 ", '%s', %" PRIu32 ", %" PRIu64 ", %u, %u);",
        file_info.file_id,
        file_info.file_name.c_str(),
        file_info.file_hash_key,
        file_info.file_size,
        file_info.num_block,
        file_info.replica);

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if(rc != SQLITE_OK)
    {
      con.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
      con.logger->info("file_metadata inserted successfully\n");
    }

    // Close Database
    sqlite3_close(db);
  }

  void Directory::insert_block_metadata(BlockInfo block_info)
  {
    // Open database
    sqlite3 *db;
    char sql[512];
    char *zErrMsg = 0;
    string path = con.settings.get<string>("path.metadata") + "/metadata.db";
    int rc = sqlite3_open(path.c_str(), &db);
    if(rc)
    {
      con.logger->error("Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
      con.logger->info("Opened database successfully\n");
    }

    // Create sql statement
    sprintf(sql, "INSERT INTO block_table (\
      file_id, block_seq, block_hash_key, block_name, \
        block_size, is_inter, node, l_node, r_node, is_commit) \
      VALUES (%" PRIu32 ", %u, %" PRIu32 ", '%s', %" PRIu32 ",\
      %u, '%s', '%s', '%s', %u);",
        block_info.file_id,
        block_info.block_seq,
        block_info.block_hash_key,
        block_info.block_name.c_str(),
        block_info.block_size,
        block_info.is_inter,
        block_info.node.c_str(),
        block_info.l_node.c_str(),
        block_info.r_node.c_str(),
        block_info.is_commit);

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if(rc != SQLITE_OK)
    {
      con.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
      con.logger->info("block_metadata inserted successfully\n");
    }

    // Close Database
    sqlite3_close(db);
  }

  void Directory::select_file_metadata(uint32_t file_id, FileInfo *file_info)
  {
    // Open database
    sqlite3 *db;
    char sql[512];
    char *zErrMsg = 0;
    string path = con.settings.get<string>("path.metadata") + "/metadata.db";
    int rc = sqlite3_open(path.c_str(), &db);
    if(rc)
    {
      con.logger->error("Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
      con.logger->info("Opened database successfully\n");
    }

    // Create sql statement
    sprintf(sql, "SELECT * from file_table where file_id=%" PRIu32 ";", file_id);

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, file_callback, (void*)file_info, &zErrMsg);
    if(rc != SQLITE_OK)
    {
      con.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
      con.logger->info("file_metadata selected successfully\n");
    }

    // Close Database
    sqlite3_close(db);
  }

  void Directory::select_block_metadata(uint32_t file_id, unsigned int block_seq, BlockInfo *block_info)
  {
    // Open database
    sqlite3 *db;
    char sql[512];
    char *zErrMsg = 0;
    string path = con.settings.get<string>("path.metadata") + "/metadata.db";
    int rc = sqlite3_open(path.c_str(), &db);
    if(rc)
    {
      con.logger->error("Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
      con.logger->info("Opened database successfully\n");
    }

    // Create sql statement
    sprintf(sql, "SELECT * from block_table where (file_id=%" PRIu32 ") and \
        (block_seq=%u);", file_id, block_seq);

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, block_callback, (void*)block_info, &zErrMsg);
    if(rc != SQLITE_OK)
    {
      con.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
      con.logger->info("block_metadata selected successfully\n");
    }

    // Close Database
    sqlite3_close(db);
  } 

  void Directory::update_file_metadata(uint32_t file_id, FileInfo file_info)
  {
    // Open database
    sqlite3 *db;
    char sql[512];
    char *zErrMsg = 0;
    string path = con.settings.get<string>("path.metadata") + "/metadata.db";
    int rc = sqlite3_open(path.c_str(), &db);
    if(rc)
    {
      con.logger->error("Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
      con.logger->info("Opened database successfully\n");
    }

    // Create sql statement
    sprintf(sql, "UPDATE file_table set \
        file_id=%" PRIu32 ", file_name='%s', file_hash_key=%" PRIu32 "\
        , file_size=%" PRIu64 ", \
        num_block=%u, replica=%u where file_id=%" PRIu32 ";",
        file_info.file_id,
        file_info.file_name.c_str(),
        file_info.file_hash_key,
        file_info.file_size,
        file_info.num_block,
        file_info.replica,
        file_id);

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if(rc != SQLITE_OK)
    {
      con.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
      con.logger->info("file_metadata updated successfully\n");
    }

    // Close Database
    sqlite3_close(db);
  }

  void Directory::update_block_metadata(uint32_t file_id, unsigned int block_seq, BlockInfo block_info)
  {
    // Open database
    sqlite3 *db;
    char sql[512];
    char *zErrMsg = 0;
    string path = con.settings.get<string>("path.metadata") + "/metadata.db";
    int rc = sqlite3_open(path.c_str(), &db);
    if(rc)
    {
      con.logger->error("Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
      con.logger->info("Opened database successfully\n");
    }

    // Create sql statement
    sprintf(sql, "UPDATE block_table set \
        file_id=%" PRIu32 ", block_seq=%u, block_hash_key=%" PRIu32 ", block_name='%s', \
        block_size=%" PRIu32 ", is_inter=%u, node='%s', l_node='%s', r_node='%s' \
        , is_commit=%u where (file_id=%" PRIu32 ") and (block_seq=%u);",
        block_info.file_id,
        block_info.block_seq,
        block_info.block_hash_key,
        block_info.block_name.c_str(),
        block_info.block_size,
        block_info.is_inter,
        block_info.node.c_str(),
        block_info.l_node.c_str(),
        block_info.r_node.c_str(),
        block_info.is_commit,
        block_info.file_id,
        block_info.block_seq);

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if(rc != SQLITE_OK)
    {
      con.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
      con.logger->info("block_metadata updated successfully\n");
    }

    // Close Database
    sqlite3_close(db);
  }

  void Directory::delete_file_metadata(uint32_t file_id)
  {
    // Open database
    sqlite3 *db;
    char sql[512];
    char *zErrMsg = 0;
    string path = con.settings.get<string>("path.metadata") + "/metadata.db";
    int rc = sqlite3_open(path.c_str(), &db);
    if(rc)
    {
      con.logger->error("Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
      con.logger->info("Opened database successfully\n");
    }

    // Create sql statement
    sprintf(sql, "DELETE from file_table where file_id=%" PRIu32 ";", file_id);

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if(rc != SQLITE_OK)
    {
      con.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
      con.logger->info("file_metadata deleted successfully\n");
    }

    // Close Database
    sqlite3_close(db);
  }

  void Directory::delete_block_metadata(uint32_t file_id, unsigned int block_seq)
  {
    // Open database
    sqlite3 *db;
    char sql[512];
    char *zErrMsg = 0;
    string path = con.settings.get<string>("path.metadata") + "/metadata.db";
    int rc = sqlite3_open(path.c_str(), &db);
    if(rc)
    {
      con.logger->error("Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
      con.logger->info("Opened database successfully\n");
    }

    // Create sql statement
    sprintf(sql, "DELETE from block_table where (file_id=%" PRIu32 ") and (block_seq=%u);", file_id, block_seq);

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if(rc != SQLITE_OK)
    {
      con.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
      con.logger->info("block_metadata deleted successfully\n");
    }

    // Close Database
    sqlite3_close(db);
  }

  void Directory::display_file_metadata()
  {
    // Open database
    sqlite3 *db;
    char sql[512];
    char *zErrMsg = 0;
    const char* data = "Callback function called";
    string path = con.settings.get<string>("path.metadata") + "/metadata.db";
    int rc = sqlite3_open(path.c_str(), &db);
    if(rc)
    {
      con.logger->error("Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
      con.logger->info("Opened database successfully\n");
    }

    // Create sql statement
    sprintf(sql, "SELECT * from file_table");

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, display_callback, (void*)data, &zErrMsg);
    if(rc != SQLITE_OK)
    {
      con.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
      con.logger->info("file_metadata displayed successfully\n");
    }

    // Close Database
    sqlite3_close(db);
  }

  void Directory::display_block_metadata()
  {
    // Open database
    sqlite3 *db;
    char sql[512];
    char *zErrMsg = 0;
    const char* data = "Callback function called";
    string path = con.settings.get<string>("path.metadata") + "/metadata.db";
    int rc = sqlite3_open(path.c_str(), &db);
    if(rc != SQLITE_OK)
    {
      con.logger->error("Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
      con.logger->info("Opened database successfully\n");
    }

    // Create sql statement
    sprintf(sql, "SELECT * from block_table");

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, display_callback, (void*)data, &zErrMsg);
    if(rc != SQLITE_OK)
    {
      con.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
      con.logger->info("block_metadata displayed successfully\n");
    }

    // Close Database
    sqlite3_close(db);
  }

  bool Directory::is_exist(uint32_t file_id)
  {
    // Open database
    sqlite3 *db;
    char sql[512];
    char *zErrMsg = 0;
    string path = con.settings.get<string>("path.metadata") + "/metadata.db";
    int rc = sqlite3_open(path.c_str(), &db);
    if(rc)
    {
      con.logger->error("Can't open block_talbe: %s\n", sqlite3_errmsg(db));
    }
    else
    {
      con.logger->info("Opened block_table successfully\n");
    }
    bool *result = new bool;

    // Create SQL statement
    sprintf(sql, "SELECT * from file_table where file_id=%" PRIu32 ";", file_id);

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, exist_callback, result, &zErrMsg);
    if(rc != SQLITE_OK)
    {
      con.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
      con.logger->info("is_exist executed successfully\n");
    }
    bool return_bool = *result;
    delete result;
    return return_bool;
  }
}
