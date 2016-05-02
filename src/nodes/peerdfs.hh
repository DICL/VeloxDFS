#pragma once

#include "node.hh"
#include "local_io.hh"
#include "../messages/block.hh"
#include "../messages/file.hh"
#include "../messages/filedescription.hh"
#include "../messages/list_files.hh"
#include "../fs/directory.hh"
#include "../common/histogram.hh"

#include <string>

namespace eclipse {

using vec_str = std::vector<std::string>;
typedef std::function<void(std::string, std::string)> req_func;

class PeerDFS: public Node {
  public:
    PeerDFS (network::Network*);
    ~PeerDFS ();

    void insert_key (uint32_t, std::string, std::string);
    void request_key (std::string, int);

    virtual void insert (uint32_t, std::string, std::string);
    virtual void request (uint32_t, std::string, req_func);

    void close ();
    bool insert_block (messages::Block*);
    bool insert_file (messages::File*);
    bool delete_block (messages::Block*);
    bool delete_file (std::string);
    bool list (messages::List_files*);
    bool format ();
    FileDescription request_file (std::string);
    bool file_exist (std::string);

  protected:
    Directory directory;
    Local_io local_io;
    std::unique_ptr<Histogram> boundaries;
    std::map<std::string, req_func> requested_blocks;
};

}
