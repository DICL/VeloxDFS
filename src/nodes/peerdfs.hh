#pragma once

#include "node.hh"
#include "../network/asyncnode.hh"
#include "../messages/blockinfo.hh"
#include "../messages/fileinfo.hh"
#include "../messages/keyrequest.hh"
#include "../messages/filerequest.hh"
#include "../messages/filedescription.hh"
#include "../messages/filelist.hh"
#include "../messages/filedel.hh"
#include "../messages/blockdel.hh"
#include "../fs/directory.hh"

#include <string>
#include <boost/asio.hpp>

namespace eclipse {

using vec_str = std::vector<std::string>;
typedef std::function<void(std::string, std::string)> req_func;

class PeerDFS: public Node, public AsyncNode {
  public:
    PeerDFS (Context&);
    ~PeerDFS ();

    bool establish () override;
    void on_read (messages::Message*) override;
    void on_connect () override;
    void on_disconnect() override;

    void insert (std::string, std::string);
    void request (std::string, req_func);
    void Delete (std::string);
    void close ();
    bool insert_block (messages::BlockInfo*);
    bool insert_file (messages::FileInfo*);
    bool delete_block (messages::BlockDel*);
    bool delete_file (messages::FileDel*);
    bool list (messages::FileList*);
    FileDescription request_file (messages::FileRequest*);

  protected:
    Directory directory;
    std::map<std::string, req_func> requested_blocks;
    bool connected = false;
    uint32_t size;
    std::string disk_path;

    template <typename T> void process (T);
};

}
