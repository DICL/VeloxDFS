#pragma once

#include "node.hh"
#include "../network/asyncnode.hh"
#include "../cache/cache.hh"
#include "../common/histogram.hh"
#include "../messages/blockinfo.hh"
#include "../messages/fileinfo.hh"
#include "../messages/keyrequest.hh"
#include "../fs/directory.hh"

#include <string>
#include <boost/asio.hpp>

namespace eclipse {

using std::string;
using std::thread;
using std::map;
using vec_str    = std::vector<std::string>;

typedef std::function<void(std::string)> req_func;

class PeerDFS: public Node, public AsyncNode {
  public:
    PeerDFS (Context&);
    ~PeerDFS ();

    bool establish () override;
    void on_read (messages::Message*) override;
    void on_connect () override;
    void on_disconnect() override;

    void insert (string, string);
    void request (string, req_func);
    void close ();
    bool insert_block (messages::BlockInfo*);
    bool insert_file (messages::FileInfo*);
    bool request_file (messages::KeyRequest*, req_func);

  protected:
    Directory directory;
    std::map<std::string, req_func> requested_blocks;
    bool connected = false;
    uint32_t size;
    string disk_path;

    template <typename T> void process (T);
};

}
