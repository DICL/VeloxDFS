#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include "../messages/blockinfo.hh"
#include "../messages/fileinfo.hh"
#include "blockmetadata.hh"

namespace eclipse {
using namespace messages;

class Directory {
  public:
    Directory();
    void create_tables();

    void file_table_insert(FileInfo&);
    void file_table_select(std::string, FileInfo *);
    void file_table_select_all(std::vector<FileInfo>&);
    void file_table_update(std::string, uint64_t, uint32_t);
    void file_table_delete(std::string);
    bool file_table_exists(std::string);
    void file_table_confirm_upload (std::string, uint32_t);

    void block_table_insert(BlockMetadata&);
    void block_table_insert_all(std::vector<BlockMetadata>&);
    void block_table_select(std::string, std::vector<BlockMetadata>&);
    void block_table_select_by_index(std::string, uint32_t, BlockMetadata*);
    void block_table_select_all(std::vector<BlockMetadata>&);
    void block_table_update(std::string, uint32_t, uint32_t);
    void block_table_delete(std::string, uint32_t);
    void block_table_delete_all(std::string);
    void select_last_block_metadata(std::string, BlockMetadata*);

  protected:
    bool query_exec_simple(char*, int (*)(void*,int,char**,char**), void*);

  private:
    std::string path;
};

}
