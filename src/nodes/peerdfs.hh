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
#include "../messages/fileexist.hh"
#include "../fs/directory.hh"
#include "../common/histogram.hh"

#include <string>
#include <boost/asio.hpp>

namespace eclipse {

using vec_str = std::vector<std::string>;
typedef std::function<void(std::string, std::string)> req_func;

class PeerDFS: public Node, public AsyncNode {
  public:
    PeerDFS ();
    ~PeerDFS ();

    bool establish () override;
    void on_read (messages::Message*, int) override;
    void on_connect () override;
    void on_disconnect(int) override;

    virtual void insert (uint32_t, std::string, std::string);
    virtual void request (uint32_t, std::string, req_func);

    void Delete (std::string);
    void close ();
    bool insert_block (messages::BlockInfo*);
    bool insert_file (messages::FileInfo*);
    bool delete_block (messages::BlockDel*);
    bool delete_file (messages::FileDel*);
    bool list (messages::FileList*);
    bool format ();
    FileDescription request_file (messages::FileRequest*);
    bool file_exist (std::string);

  protected:
    Directory directory;
    std::unique_ptr<Histogram> boundaries;
    std::map<std::string, req_func> requested_blocks;
    bool connected = false;
    int size;
    std::string disk_path;

    template <typename T> void process (T);
};

}
