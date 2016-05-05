#pragma once

#include "dio.hh"
#include "fs.hh"
#include "../fs/directory.hh"
#include <string>

namespace eclipse {

class DFS: public FS {
  public:
    DFS (DIO*);
    ~DFS ();

    bool insert_block (messages::Block*) override;
    bool insert_file (messages::File*) override;
    bool delete_block (messages::Block*) override;
    bool delete_file (std::string) override;
    bool list (messages::List_files*) override;
    bool format () override;
    messages::FileDescription request_file (std::string) override;
    bool file_exist (std::string) override;
    void insert_key (uint32_t, std::string, std::string) override;
    void request_key (std::string, int) override;
    void request (uint32_t, std::string, req_func) override;

  protected:
    Directory directory;
    DIO* dio;
};

}
