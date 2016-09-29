#include "directory.hh"
using namespace std;

namespace eclipse {
// Directory {{{
  Directory::Directory() {
    path = context.settings.get<string>("path.scratch") + "/metadata.db";
    zErrMsg = 0;
  }
// }}}
// ~Directory {{{
  Directory::~Directory() {
  }
// }}}
// file_callback {{{
  int Directory::file_callback(void *list, int argc, char **argv, char **azColName) {
    auto file_list = reinterpret_cast<vector<FileInfo>*>(list);
    for (int i=0; i<argc; i+=6) {
      FileInfo file;
      file.name = argv[i];
      file.hash_key = atoi(argv[i+1]);
      file.size = atoll(argv[i+2]);
      file.num_block = atoll(argv[i+3]);
      file.type = argv[i+4];
      file.replica = atoi(argv[i+5]);
      file_list->push_back(file);
    }
    return 0;
  }
// }}}
// block_callback {{{
  int Directory::block_callback(void *list, int argc, char **argv, char **azColName) {
    auto block_list = reinterpret_cast<vector<BlockInfo>*>(list);
    for (int i=0; i<argc; i+=7) {
      BlockInfo block;
      block.name = argv[i];
      block.hash_key = atoi(argv[i+1]);
      block.file_name = argv[i+2];
      block.seq = atoll(argv[i+3]);
      block.size = atoll(argv[i+4]);
      block.net_id = atoi(argv[i+5]);
      block.check_commit = atoi(argv[i+6]);
      block_list->push_back(block);
    }
    return 0;
  }
// }}}
// slice_callback {{{
  int Directory::slice_callback(void *info, int argc, char **argv, char **azColName) {
    auto slice = reinterpret_cast<SliceInfo*>(info);
    slice->name = argv[0];
    slice->hash_key = atoi(argv[1]);
    slice->size = atoll(argv[2]);
    return 0;
  } 
// }}}
// mdlist_callback {{{
  int Directory::mdlist_callback(void *list, int argc, char **argv, char **azColName) {
    auto mdlist_list = reinterpret_cast<vector<MdlistInfo>*>(list);
    for (int i=0; i<argc; i+=5) {
      MdlistInfo mdlist;
      mdlist.file_name = argv[i];
      mdlist.slice_name = argv[i+1];
      mdlist.file_pos = atoll(argv[i+2]);
      mdlist.slice_pos = atoll(argv[i+3]);
      mdlist.len = atoll(argv[i+4]);
      mdlist_list->push_back(mdlist);
    }
    return 0;
  }
// }}}
// node_callback {{{
  int Directory::node_callback(void *list, int argc, char **argv, char **azColName) {
    auto node_list = reinterpret_cast<vector<NodeInfo>*>(list);
    for (int i=0; i<argc; i+=4) {
      NodeInfo node;
      node.name = argv[i];
      node.file_name = argv[i+1];
      node.id = atoi(argv[i+2]);
      node.net_id = atoi(argv[i+3]);
      node_list->push_back(node);
    }
    return 0;
  }
// }}}
// display_callback {{{
  int Directory::display_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    for (int i=0; i<argc; i++)
      context.logger->info("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    printf("\n");
    return 0;
  }
// }}}
// exist_callback {{{
  int Directory::exist_callback(void *result, int argc, char **argv, char **azColName) {
    *reinterpret_cast<bool*>(result) = argv[0] ? true : false;
    return 0;
  }
// }}}
// open_db {{{
  void Directory::open_db() {
    mutex.lock();
    rc = sqlite3_open(path.c_str(), &db);
    if (rc) {
      context.logger->error("Can't open database: %s\n", sqlite3_errmsg(db));
    } else {
      //context.logger->info("Opened database successfully\n");
    }
    mutex.unlock();
  }
// }}}
// init_db {{{
  void Directory::init_db() {
    open_db();
    mutex.lock();

    // Create File Table
    sprintf(sql, "CREATE TABLE FILE_TABLE( \
        NAME       TEXT  NOT NULL, \
        HASH_KEY   INT   NOT NULL, \
        SIZE       INT   NOT NULL, \
        NUM_BLOCK  INT   NOT NULL, \
        TYPE       TEXT  NOT NULL, \
        REPLICA    INT   NOT NULL, \
        PRIMARY KEY (NAME));"); 

    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if(rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("FILE_TABLE created successfully\n");
    }

    // Create Block Table
    sprintf(sql, "CREATE TABLE BLOCK_TABLE( \
        NAME           TEXT      NOT NULL, \
        HASH_KEY       INT       NOT NULL, \
        FILE_NAME      TEXT      NOT NULL, \
        SEQ            INT       NOT NULL, \
        SIZE           INT       NOT NULL, \
        NET_ID         INT       NOT NULL, \
        CHECK_COMMIT   INT       NOT NULL, \
        PRIMARY KEY (NAME));"); 

    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if(rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("BLOCK_TABLE created successfully\n");
    }

    // Create Slice Table
    sprintf(sql, "CREATE TABLE SLICE_TABLE( \
        NAME          TEXT      NOT NULL, \
        HASH_KEY      INT       NOT NULL, \
        SIZE          INT       NOT NULL, \
        PRIMARY KEY (NAME));"); 

    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if(rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("SLICE_TABLE created successfully\n");
    }

    // Create Metadata List Table
    sprintf(sql, "CREATE TABLE MDLIST_TABLE( \
        FILE_NAME     TEXT      NOT NULL, \
        SLICE_NAME    TEXT      NOT NULL, \
        FILE_POS      INT       NOT NULL, \
        SLICE_POS     INT       NOT NULL, \
        LEN           INT       NOT NULL, \
        PRIMARY KEY (FILE_NAME, FILE_POS));"); 

    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if(rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("MDLIST_TABLE created successfully\n");
    }

    // Create Node Table
    sprintf(sql, "CREATE TABLE NODE_TABLE( \
        NAME         TEXT      NOT NULL, \
        FILE_NAME    TEXT      NOT NULL, \
        ID           INT       NOT NULL, \
        NET_ID       INT       NOT NULL, \
        PRIMARY KEY (NAME, ID));"); 

    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if(rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("NODE_TABLE created successfully\n");
    }
    sqlite3_close(db);
    mutex.unlock();
  }
// }}}
// insert_file_metadata {{{
  void Directory::insert_file_metadata(FileInfo* file_info) {
    open_db();
    sprintf(sql, "INSERT INTO FILE_TABLE (\
      NAME, HASH_KEY, SIZE, NUM_BLOCK, TYPE, REPLICA)\
      VALUES('%s', %" PRIu32 ", %" PRIu64 ", %" PRIu64 ", '%s', %d);",
        file_info->name.c_str(),
        file_info->hash_key,
        file_info->size,
        file_info->num_block,
        file_info->type.c_str(),
        file_info->replica);

    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("file_metadata inserted successfully\n");
    }

    sqlite3_close(db);
  }
// }}}
// insert_block_metadata {{{
  void Directory::insert_block_metadata(BlockInfo* block_info) {
    open_db();
    sprintf(sql, "INSERT INTO BLOCK_TABLE (\
      NAME, HASH_KEY, FILE_NAME, SEQ, SIZE, NET_ID, CHECK_COMMIT)\
      VALUES ('%s', %" PRIu32 ", '%s', %" PRIu64 ", %" PRIu64 ", %d, %d)",
        block_info->name.c_str(),
        block_info->hash_key,
        block_info->file_name.c_str(),
        block_info->seq,
        block_info->size,
        block_info->net_id,
        block_info->check_commit);

    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("block_metadata inserted successfully\n");
    }
    sqlite3_close(db);
  }
// }}}
// insert_slice_metadata {{{
  void Directory::insert_slice_metadata(SliceInfo* slice_info) {
    open_db();
    sprintf(sql, "INSERT INTO SLICE_TABLE (\
      NAME, HASH_KEY, SIZE)\
      VALUES ('%s', %" PRIu32 ", %" PRIu64 ");",
        slice_info->name.c_str(),
        slice_info->hash_key,
        slice_info->size);
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("slice_metadata inserted successfully\n");
    }
    sqlite3_close(db);
  }
// }}}
// insert_mdlist_metadata {{{
  void Directory::insert_mdlist_metadata(MdlistInfo* mdlist_info) {
    open_db();
    sprintf(sql, "INSERT INTO MDLIST_TABLE (\
      FILE_NAME, SLICE_NAME, FILE_POS, SLICE_POS, LEN)\
      VALUES ('%s', '%s', %" PRIu64 ", %" PRIu64 ", %" PRIu64 ");",
        mdlist_info->file_name.c_str(),
        mdlist_info->slice_name.c_str(),
        mdlist_info->file_pos,
        mdlist_info->slice_pos,
        mdlist_info->len);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("mdlist_metadata inserted successfully\n");
    }
    sqlite3_close(db);
  }
// }}}
// insert_node_metadata {{{
  void Directory::insert_node_metadata(NodeInfo* node_info) {
    open_db();
    sprintf(sql, "INSERT INTO NODE_TABLE (\
      NAME, FILE_NAME, ID, NET_ID)\
      VALUES ('%s', '%s', %d, %d);",
        node_info->name.c_str(),
        node_info->file_name.c_str(),
        node_info->id,
        node_info->net_id);
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("node_metadata inserted successfully\n");
    }
    sqlite3_close(db);
  }
// }}}
// select_file_metadata {{{
  void Directory::select_file_metadata(string name, FileInfo* file_info) {
    open_db();
    vector<FileInfo> file_list;
    sprintf(sql, "SELECT * FROM FILE_TABLE WHERE NAME='%s';", name.c_str());
    rc = sqlite3_exec(db, sql, file_callback, (void*)&file_list, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      *file_info = file_list[0];
      //context.logger->info("file_metadata selected successfully\n");
    }
    sqlite3_close(db);
  }
// }}}
// select_block_metadata {{{
  void Directory::select_block_metadata(string file_name, vector<BlockInfo>* block_list) {
    open_db();
    sprintf(sql, "SELECT * FROM BLOCK_TABLE WHERE FILE_NAME='%s' ORDER BY SEQ;", file_name.c_str());
    //sprintf(sql, "SELECT * FROM BLOCK_TABLE WHERE FILE_NAME='%s';", file_name.c_str());
    rc = sqlite3_exec(db, sql, block_callback, (void*)block_list, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("block_metadata selected successfully\n");
    }
    sqlite3_close(db);
  } 
// }}}
// select_slice_metadata {{{
  void Directory::select_slice_metadata(string name, SliceInfo* slice_info) {
    open_db();
    sprintf(sql, "SELECT * FROM SLICE_TABLE WHERE NAME='%s';", name.c_str());
    rc = sqlite3_exec(db, sql, slice_callback, (void*)slice_info, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("slice_metadata selected successfully\n");
    }
    sqlite3_close(db);
  } 
// }}}
// select_mdlist_metadata {{{
  void Directory::select_mdlist_metadata(string file_name, vector<MdlistInfo>* mdlist_list) {
    open_db();
    sprintf(sql, "SELECT * FROM MDLIST_TABLE WHERE FILE_NAME='%s' ORDER BY FILE_POS;", file_name.c_str());
    rc = sqlite3_exec(db, sql, mdlist_callback, (void*)mdlist_list, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("mdlist_metadata selected successfully\n");
    }
    sqlite3_close(db);
  } 
// }}}
// select_node_metadata {{{
  void Directory::select_node_metadata(string name, vector<NodeInfo>* node_list) {
    open_db();
    sprintf(sql, "SELECT * FROM NODE_TABLE WHERE NAME='%s' ORDER BY ID;", name.c_str());
    rc = sqlite3_exec(db, sql, node_callback, (void*)node_list, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("node_metadata selected successfully\n");
    }
    sqlite3_close(db);
  } 
// }}}
// select_rm_node_metadata {{{
  void Directory::select_rm_node_metadata(string file_name, vector<NodeInfo>* node_list) {
    open_db();
    sprintf(sql, "SELECT * FROM NODE_TABLE WHERE FILE_NAME='%s' ORDER BY NET_ID;", file_name.c_str());
    rc = sqlite3_exec(db, sql, node_callback, (void*)node_list, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("rm_node_metadata selected successfully\n");
    }
    sqlite3_close(db);
  } 
// }}}
// select_one_node_metadata {{{
  void Directory::select_one_node_metadata(string name, NodeInfo* node_info) {
    open_db();
    vector<NodeInfo> node_list;
    sprintf(sql, "SELECT * FROM NODE_TABLE WHERE (NAME='%s') AND (ID=0);", name.c_str());
    rc = sqlite3_exec(db, sql, node_callback, (void*)&node_list, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      *node_info = node_list[0];
      //context.logger->info("one_node_metadata selected successfully\n");
    }
    sqlite3_close(db);
  } 
// }}}
// select_all_file_metadata {{{
  void Directory::select_all_file_metadata(vector<FileInfo>* file_list) {
    open_db();
    mutex.lock();
    sprintf(sql, "SELECT * FROM FILE_TABLE;");
    rc = sqlite3_exec(db, sql, file_callback, (void*)file_list, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("file_metadata selected successfully\n");
    }
    sqlite3_close(db);
    mutex.unlock();
  }
// }}}
// select_all_block_metadata {{{
  void Directory::select_all_block_metadata(vector<BlockInfo>* block_list) {
    open_db();
    sprintf(sql, "SELECT * FROM BLOCK_TABLE;");
    rc = sqlite3_exec(db, sql, block_callback, (void*)block_list, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("block_metadata selected successfully\n");
    }
    sqlite3_close(db);
  } 
// }}}
// update_file_metadata {{{
  void Directory::update_file_metadata(FileUpdate* file_update) {
    open_db();
    sprintf(sql, "UPDATE FILE_TABLE SET \
        SIZE=%" PRIu64 ", NUM_BLOCK=%" PRIu64 " WHERE NAME='%s';",
        file_update->size,
        file_update->num_block,
        file_update->name.c_str());
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("file_metadata updated successfully\n");
    }
    sqlite3_close(db);
  }
// }}}
// update_block_metadata {{{
  void Directory::update_block_metadata(BlockUpdate* block_update) {
    open_db();
    sprintf(sql, "UPDATE BLOCK_TABLE SET SIZE=%" PRIu64 " WHERE NAME='%s';",
        block_update->len,
        block_update->name.c_str());
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("block_metadata updated successfully\n");
    }
    sqlite3_close(db);
  }
// }}}
// update_mdlist_metadata {{{
  void Directory::update_mdlist_metadata(MdlistUpdate* mdlist_update) {
    open_db();
    sprintf(sql, "UPDATE MDLIST_TABLE SET \
        SIZE=%" PRIu64 " WHERE (FILE_NAME='%s') AND (SLICE_NAME='%s');",
        mdlist_update->len,
        mdlist_update->file_name.c_str(),
        mdlist_update->slice_name.c_str());
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("mdlist_metadata updated successfully\n");
    }
    sqlite3_close(db);
  }
// }}}
// update_node_metadata {{{
  void Directory::update_node_metadata(NodeUpdate* node_update) {
    open_db();
    sprintf(sql, "UPDATE NODE_TABLE SET \
        NET_ID=%d WHERE (NAME='%s') AND (ID=%d);",
        node_update->net_id,
        node_update->name.c_str(),
        node_update->id);
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("node_metadata updated successfully\n");
    }
    sqlite3_close(db);
  }
// }}}
// delete_file_metadata {{{
  void Directory::delete_file_metadata(string name) {
    open_db();
    sprintf(sql, "DELETE FROM FILE_TABLE WHERE NAME='%s';", name.c_str());
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("file_metadata deleted successfully\n");
    }
    sqlite3_close(db);
  }
// }}}
// delete_block_metadata {{{
  void Directory::delete_block_metadata(string file_name)
  {
    open_db();
    sprintf(sql, "DELETE FROM BLOCK_TABLE WHERE FILE_NAME='%s';", file_name.c_str());
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("block_metadata deleted successfully\n");
    }
    sqlite3_close(db);
  }
// }}}
// delete_slice_metadata {{{
  void Directory::delete_slice_metadata(string name)
  {
    open_db();
    sprintf(sql, "DELETE FROM SLICE_TABLE WHERE NAME='%s';", name.c_str());
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("slice_metadata deleted successfully\n");
    }
    sqlite3_close(db);
  }
// }}}
// delete_mdlist_metadata {{{
  void Directory::delete_mdlist_metadata(string file_name)
  {
    open_db();
    sprintf(sql, "DELETE FROM MDLIST_TABLE WHERE FILE_NAME='%s';", file_name.c_str());
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("mdlist_metadata deleted successfully\n");
    }
    sqlite3_close(db);
  }
// }}}
// delete_node_metadata {{{
  void Directory::delete_node_metadata(string file_name)
  {
    open_db();
    sprintf(sql, "DELETE FROM NODE_TABLE WHERE FILE_NAME='%s';", file_name.c_str());
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("node_metadata deleted successfully\n");
    }
    sqlite3_close(db);
  }
// }}}
// display_file_metadata {{{
  void Directory::display_file_metadata()
  {
    open_db();
    sprintf(sql, "SELECT * FROM FILE_TABLE");
    rc = sqlite3_exec(db, sql, display_callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("file_metadata displayed successfully\n");
    }
    sqlite3_close(db);
  }
// }}}
// display_block_metadata {{{
  void Directory::display_block_metadata() {
    open_db();
    sprintf(sql, "SELECT * FROM BLOCK_TABLE");
    rc = sqlite3_exec(db, sql, display_callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("block_metadata displayed successfully\n");
    }
    sqlite3_close(db);
  }
// }}}
// check_exist {{{
  bool Directory::check_exist(string name, string type) {
    bool result = false;
    open_db();
    if (!type.compare("file")) 
        sprintf(sql, "SELECT NAME FROM FILE_TABLE WHERE NAME='%s';", name.c_str());
    else if (!type.compare("block"))
        sprintf(sql, "SELECT NAME FROM BLOCK_TABLE WHERE NAME='%s';", name.c_str());
    else if (!type.compare("slice"))
        sprintf(sql, "SELECT NAME FROM SLICE_TABLE WHERE NAME='%s';", name.c_str());
    rc = sqlite3_exec(db, sql, exist_callback, &result, &zErrMsg);
    if (rc != SQLITE_OK) {
      context.logger->error("SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } else {
      //context.logger->info("check_exist executed successfully\n");
    }
    sqlite3_close(db);
    return result;
  }
}
