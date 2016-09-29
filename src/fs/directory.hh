#pragma once
#include <iostream>
#include <string>
#include <sqlite3.h>
#include <cstdio>
#include <cstdint>
#include <cinttypes>
#include <vector>
#include <mutex>
#include "../common/context_singleton.hh"
#include "../messages/fileinfo.hh"
#include "../messages/fileupdate.hh"
#include "../messages/filedel.hh"
#include "../messages/blockinfo.hh"
#include "../messages/blockupdate.hh"
#include "../messages/blockdel.hh"
#include "../messages/sliceinfo.hh"
#include "../messages/slicedel.hh"
#include "../messages/mdlistinfo.hh"
#include "../messages/mdlistupdate.hh"
#include "../messages/mdlistdel.hh"
#include "../messages/nodeinfo.hh"
#include "../messages/nodeupdate.hh"
#include "../messages/nodedel.hh"

namespace eclipse {
  using namespace messages;
  class Directory {
    private:
      std::mutex mutex;
      sqlite3 *db;
      char *zErrMsg;
      std::string path;
      int rc;
      char sql[256];
      static int file_callback(void*, int, char**, char**);
      static int block_callback(void*, int, char**, char**);
      static int slice_callback(void*, int, char**, char**);
      static int mdlist_callback(void*, int, char**, char**);
      static int node_callback(void*, int, char**, char**);
      static int display_callback(void*, int, char**, char**);
      static int exist_callback(void*, int, char**, char**);

    public:
      Directory();
      ~Directory();
      void open_db();
      void init_db();
      void insert_file_metadata(FileInfo*);
      void insert_block_metadata(BlockInfo*);
      void insert_slice_metadata(SliceInfo*);
      void insert_mdlist_metadata(MdlistInfo*);
      void insert_node_metadata(NodeInfo*);
      void select_file_metadata(std::string, FileInfo*);
      void select_block_metadata(std::string, std::vector<BlockInfo>*);
      void select_slice_metadata(std::string, SliceInfo*);
      void select_mdlist_metadata(std::string, std::vector<MdlistInfo>*);
      void select_node_metadata(std::string, std::vector<NodeInfo>*);
      void select_rm_node_metadata(std::string, std::vector<NodeInfo>*);
      void select_one_node_metadata(std::string, NodeInfo*);
      void select_all_file_metadata(std::vector<FileInfo>*);
      void select_all_block_metadata(std::vector<BlockInfo>*);
      void update_file_metadata(FileUpdate*);
      void update_block_metadata(BlockUpdate*);
      void update_mdlist_metadata(MdlistUpdate*);
      void update_node_metadata(NodeUpdate*);
      void delete_file_metadata(std::string);
      void delete_block_metadata(std::string);
      void delete_slice_metadata(std::string);
      void delete_mdlist_metadata(std::string);
      void delete_node_metadata(std::string);
      void display_file_metadata();
      void display_block_metadata();
      bool check_exist(std::string, std::string);
  };
}
