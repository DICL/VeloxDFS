#pragma once
#include <iostream>
#include <string>
#include <sqlite3.h>
#include <cstdio>
#include <cstdint>
#include <cinttypes>
#include <vector>
#include "../common/context.hh"
#include "../messages/blockinfo.hh"
#include "../messages/fileinfo.hh"
#include "../messages/idatainfo.hh"
#include "../messages/igroupinfo.hh"
#include "../messages/iblockinfo.hh"

namespace eclipse {
  using namespace messages;
  class Directory {
    private:
      sqlite3 *db;
      char *zErrMsg;
      std::string path;
      int rc;
      char sql[512];
      static int file_callback(void *file_info, int argc, char **argv, char **azColName);
      static int block_callback(void *block_info, int argc, char **argv, char **azColName);
      static int display_callback(void *NotUsed, int argc, char **argv, char **azColName);
      static int file_list_callback(void *list, int argc, char **argv, char **azColName);
      static int block_list_callback(void *list, int argc, char **argv, char **azColName);
      static int exist_callback(void *result, int argc, char **argv, char **azColName);

    public:
      Directory();
      ~Directory();
      void open_db();
      void init_db();
      void insert_file_metadata(FileInfo file_info);
      void insert_block_metadata(BlockInfo block_info);
      void select_file_metadata(std::string file_name, FileInfo *file_info);
      void select_block_metadata(std::string file_name, unsigned int block_seq, BlockInfo *block_info);
      void select_all_file_metadata(std::vector<FileInfo> &file_list);
      void select_all_block_metadata(std::vector<BlockInfo> &block_list);
      void update_file_metadata(std::string file_name, FileInfo file_info);
      void update_block_metadata(std::string file_name, unsigned int block_seq, BlockInfo block_info);
      void delete_file_metadata(std::string file_name);
      void delete_block_metadata(std::string file_name, unsigned int block_seq);
      void display_file_metadata();
      void display_block_metadata();
      bool is_exist(std::string file_name);
      void insert_idata_metadata(IDataInfo idata_info);
      void insert_igroup_metadata(IGroupInfo igroup_info);
      void insert_iblock_metadata(IBlockInfo iblock_info);
  };
}
