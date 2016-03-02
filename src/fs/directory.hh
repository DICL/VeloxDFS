#pragma once
#include <iostream>
#include <string>
#include <sqlite3.h>
#include <cstdio>
#include <cstdint>
#include <cinttypes>
#include "../common/context.hh"
#include "blockinfo.hh"
#include "fileinfo.hh"

namespace eclipse {
  class Directory {
    private:
      Context con;
      static int file_callback(void *file_info, int argc, char **argv, char **azColName);
      static int block_callback(void *block_info, int argc, char **argv, char **azColName);
      static int display_callback(void *data, int argc, char **argv, char **azColName);
      static int exist_callback(void *result, int argc, char **argv, char **azColName);

    public:
      Directory();
      ~Directory();
      void init_db();
      void insert_file_metadata(FileInfo file_info);
      void insert_block_metadata(BlockInfo block_info);
      void select_file_metadata(uint32_t file_id, FileInfo *file_info);
      void select_block_metadata(uint32_t file_id, unsigned int block_seq, BlockInfo *block_info);
      void update_file_metadata(uint32_t file_id, FileInfo file_info);
      void update_block_metadata(uint32_t file_id, unsigned int block_seq, BlockInfo block_info);
      void delete_file_metadata(uint32_t file_id);
      void delete_block_metadata(uint32_t file_id, unsigned int block_seq);
      void display_file_metadata();
      void display_block_metadata();
      bool is_exist(uint32_t file_id);
  };
}
