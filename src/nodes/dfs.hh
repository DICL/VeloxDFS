#pragma once

#include "dio.hh"
#include "../messages/file.hh"
#include "../messages/block.hh"
#include "../messages/file_long.hh"
#include "../messages/file_list.hh"
#include "../fs/directory.hh"
#include <string>

namespace eclipse {

class DFS {
  public:
    DFS (DIO*);
    ~DFS ();

    bool insert_block (messages::Block*);
    bool insert_file (messages::File*);
    bool delete_block (messages::Block*);
    bool delete_file (std::string);
    bool list (messages::FileList*);
    bool format ();
    FileDescription request_file (std::string);
    bool file_exist (std::string);

  protected:
    Directory directory;
    DIO* DIO_;
};

}
