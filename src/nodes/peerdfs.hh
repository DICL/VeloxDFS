#pragma once

#include "node.hh"
#include "local_io.hh"
#include "../network/asyncnode.hh"
#include "../messages/blockinfo.hh"
#include "../messages/blockupdate.hh"
#include "../messages/fileinfo.hh"
#include "../messages/fileupdate.hh"
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
    PeerDFS (network::Network*);
    ~PeerDFS ();

    void on_read (messages::Message*, int) override;
    void on_connect () override;
    void on_disconnect(int) override;

    virtual void insert (uint32_t, std::string, std::string&);
    virtual void update (uint32_t, std::string, std::string, uint32_t, uint32_t);
    virtual void request (uint32_t, std::string, req_func);

    void close ();
    bool insert_block (messages::BlockInfo*);
    bool update_block (messages::BlockUpdate*);
    bool insert_file (messages::FileInfo*);
    bool update_file (messages::FileUpdate*);
    bool delete_block (messages::BlockDel*);
    bool delete_file (messages::FileDel*);
    bool list (messages::FileList*);
    bool format ();
    FileDescription request_file (messages::FileRequest*);
    bool file_exist (std::string);

  protected:
    Directory directory;
    Local_io local_io;
    std::unique_ptr<Histogram> boundaries;
    std::map<std::string, req_func> requested_blocks;
    int network_size;

    template <typename T> void process (T);
};

}
