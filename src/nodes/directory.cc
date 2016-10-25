#include "directory.hh"
using namespace std;

namespace eclipse {
  Directory::Directory() {
    path = context.settings.get<string>("path.metadata") + "/metadata.db";
    zErrMsg = 0;
  }

  Directory::~Directory() {
  }

  void Directory::open_db() {
    mutex.lock();
    rc = sqlite3_open(path.c_str(), &db);
    if (rc) {
      context.logger->error("Can't open database: %s\n", sqlite3_errmsg(db));
    } else {
      context.logger->info("Opened database successfully\n");
    }
    mutex.unlock();
  }

  int Directory::file_callback(void *file_info, int argc, char **argv, char **azColName) {
    int i = 0;
    auto file = reinterpret_cast<FileInfo*>(file_info);
    file->name          = argv[i++];
    file->hash_key      = atoi(argv[i++]);
    file->size          = atoll(argv[i++]);
    file->num_block     = atoi(argv[i++]);
    file->type          = atoi(argv[i++]);
    file->replica       = atoi(argv[i]);
    return 0;
  }

  int Directory::block_callback(void *block_info, int argc, char **argv, char **azColName) {
    int i = 0;
    auto block = reinterpret_cast<BlockInfo*>(block_info);
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

  int Directory::display_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    for (int i=0; i<argc; i++)
      context.logger->info("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    printf("\n");
    return 0;
  }

  int Directory::file_list_callback(void *list, int argc, char **argv, char **azColName) {
    auto file_list = reinterpret_cast<vector<FileInfo>*>(list);
    for (int i=0; i<argc; i++) {
      FileInfo tmp_file;
      tmp_file.name      = argv[i++];
      tmp_file.hash_key  = atoi(argv[i++]);
      tmp_file.size      = atoll(argv[i++]);
      tmp_file.num_block = atoi(argv[i++]);
      tmp_file.type      = atoi(argv[i++]);
      tmp_file.replica   = atoi(argv[i]);
      file_list->push_back(tmp_file);
    }
    return 0;
  }

  int Directory::block_list_callback(void *list, int argc, char **argv, char **azColName) {
    auto block_list = reinterpret_cast<vector<BlockInfo>*>(list);
    for (int i=0; i<argc; i++) {
      BlockInfo tmp_block;
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

  int Directory::exist_callback(void *result, int argc, char **argv, char **azColName) {
    *reinterpret_cast<bool*>(result) = argv[0] ? true : false;
    return 0;
  }

  void Directory::init_db() {
    open_db();
    mutex.lock();
    sprintf(sql, "CREATE TABLE file_table( \
        name       TEXT  NOT NULL, \
        hash_key   INT   NOT NULL, \
        size       INT   NOT NULL, \
        num_block  INT   NOT NULL, \
        type       INT   NOT NULL, \
        replica    INT   NOT NULL, \
        PRIMARY KEY (name));"); 

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if(rc != SQLITE_OK)
    {
      if (rc != SQLITE_ERROR)
        context.logger->error("SQL error: %s\n", zErrMsg);

      sqlite3_free(zErrMsg);
    } else {
      context.logger->info("file_table created successfully\n");
    }

    sprintf(sql, "CREATE TABLE block_table( \
        name       TEXT      NOT NULL, \
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

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if(rc != SQLITE_OK)
    {
      if (rc != SQLITE_ERROR)
        context.logger->error("SQL error: %s\n", zErrMsg);

      sqlite3_free(zErrMsg);
    } else {
      context.logger->info("block_table created successfully\n");
    }
    sqlite3_close(db);
    mutex.unlock();
  }

  void Directory::insert_file_metadata(FileInfo &file_info) {
    open_db();
    sprintf(sql, "INSERT INTO file_table (\
      name, hash_key, size, num_block, type, replica)\
      VALUES('%s', %" PRIu32 ", %" PRIu64 ", %u, %u, %u);",
        file_info.name.c_str(),
        file_info.hash_key,
        file_info.size,
        file_info.num_block,
        file_info.type,
        file_info.replica);

    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      context.logger->info("file_metadata inserted successfully\n");
    }

    sqlite3_close(db);
  }

  void Directory::insert_block_metadata(BlockInfo* block_info) {
    open_db();
    sprintf(sql, "INSERT INTO block_table (\
      name, file_name, seq, hash_key, size, type, replica, node, l_node, r_node, is_committed)\
      VALUES ('%s', '%s', %u, %" PRIu32 ", %" PRIu32 ", %u, %u, '%s', '%s', '%s', %u);",
        block_info->name.c_str(),
        block_info->file_name.c_str(),
        block_info->seq,
        block_info->hash_key,
        block_info->size,
        block_info->type,
        block_info->replica,
        block_info->node.c_str(),
        block_info->l_node.c_str(),
        block_info->r_node.c_str(),
        block_info->is_committed);

    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      context.logger->info("block_metadata inserted successfully\n");
    }
    sqlite3_close(db);
  }

  void Directory::select_file_metadata(string name, FileInfo *file_info) {
    open_db();
    sprintf(sql, "SELECT * from file_table where name='%s';", name.c_str());
    rc = sqlite3_exec(db, sql, file_callback, (void*)file_info, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      context.logger->info("file_metadata selected successfully\n");
    }
    sqlite3_close(db);
  }

  void Directory::select_block_metadata(string file_name, unsigned int block_seq, BlockInfo *block_info) {
    open_db();
    sprintf(sql, "SELECT * from block_table where (file_name='%s') and \
        (seq=%u);", file_name.c_str(), block_seq);
    rc = sqlite3_exec(db, sql, block_callback, (void*)block_info, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      context.logger->info("block_metadata selected successfully\n");
    }
    sqlite3_close(db);
  } 

  void Directory::select_all_file_metadata(vector<FileInfo> &file_list) {
    open_db();
    mutex.lock();
    sprintf(sql, "SELECT * from file_table;");
    rc = sqlite3_exec(db, sql, file_list_callback, (void*)&file_list, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      context.logger->info("file_metadata selected successfully\n");
    }

    sqlite3_close(db);
    mutex.unlock();
  }

  void Directory::select_all_block_metadata(vector<BlockInfo> &block_info) {
    open_db();
    sprintf(sql, "SELECT * from block_table;");
    rc = sqlite3_exec(db, sql, block_list_callback, (void*)&block_info, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      context.logger->info("block_metadata selected successfully\n");
    }

    sqlite3_close(db);
  } 

  void Directory::update_file_metadata(FileUpdate &file_update) {
    open_db();
    sprintf(sql, "UPDATE file_table set \
        size=%" PRIu64 ", num_block=%u where name='%s';",
        file_update.size,
        file_update.num_block,
        file_update.name.c_str());

    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      context.logger->info("file_metadata updated successfully\n");
    }
    sqlite3_close(db);
  }

  void Directory::update_block_metadata(BlockUpdate &block_update) {
    open_db();
    sprintf(sql, "UPDATE block_table set \
        size=%" PRIu32 " where (file_name='%s') and (seq=%u);",
        block_update.size,
        block_update.file_name.c_str(),
        block_update.seq);
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      context.logger->info("block_metadata updated successfully\n");
    }
    sqlite3_close(db);
  }

  void Directory::delete_file_metadata(string name) {
    open_db();
    sprintf(sql, "DELETE from file_table where name='%s';", name.c_str());
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      context.logger->info("file_metadata deleted successfully\n");
    }
    sqlite3_close(db);
  }

  void Directory::delete_block_metadata(string file_name, unsigned int seq)
  {
    open_db();
    sprintf(sql, "DELETE from block_table where (file_name='%s') and (seq=%u);", file_name.c_str(), seq);
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      context.logger->info("block_metadata deleted successfully\n");
    }
    sqlite3_close(db);
  }

  void Directory::display_file_metadata()
  {
    open_db();
    sprintf(sql, "SELECT * from file_table");
    rc = sqlite3_exec(db, sql, display_callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      context.logger->info("file_metadata displayed successfully\n");
    }
    sqlite3_close(db);
  }

  void Directory::display_block_metadata() {
    open_db();
    sprintf(sql, "SELECT * from block_table");
    rc = sqlite3_exec(db, sql, display_callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      context.logger->info("block_metadata displayed successfully\n");
    }
    sqlite3_close(db);
  }

  bool Directory::file_exist(string name) {
    bool result = false;
    open_db();
    sprintf(sql, "SELECT name from file_table where name='%s';", name.c_str());
    rc = sqlite3_exec(db, sql, exist_callback, &result, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      context.logger->info("file_exist executed successfully\n");
    }
    sqlite3_close(db);
    return result;
  }
}
