#pragma once

// common
#include "local_io.hh"
#include "node.hh"
#include "../fs/directory.hh"
#include "../messages/checkexist.hh"
#include "../messages/keyrequest.hh"
#include "../messages/metadata.hh"
#include "../network/asyncnode.hh"
// block
#include "../messages/blockdel.hh"
#include "../messages/blockinfo.hh"
#include "../messages/blocklist.hh"
#include "../messages/blockrequest.hh"
#include "../messages/blockupdate.hh"
// file
#include "../messages/filedel.hh"
#include "../messages/filedescription.hh"
#include "../messages/fileinfo.hh"
#include "../messages/filelist.hh"
#include "../messages/filerequest.hh"
#include "../messages/fileupdate.hh"
// mdlist
#include "../messages/mdlistdel.hh"
#include "../messages/mdlistinfo.hh"
#include "../messages/mdlistlist.hh"
#include "../messages/mdlistrequest.hh"
#include "../messages/mdlistupdate.hh"
// node
#include "../messages/nodedel.hh"
#include "../messages/nodedescription.hh"
#include "../messages/nodeinfo.hh"
#include "../messages/nodelist.hh"
#include "../messages/noderequest.hh"
#include "../messages/nodeupdate.hh"
// slice
#include "../messages/slicedel.hh"
#include "../messages/sliceinfo.hh"
#include "../messages/slicelist.hh"
#include "../messages/slicerequest.hh"
// lib
#include <string>
#include <boost/asio.hpp>
#include <cstdint>

namespace eclipse {
  using vec_str = std::vector<std::string>;
  typedef std::function<void(std::string, std::string)> req_func;
  class PeerDFS: public Node, public AsyncNode {
    public:
      PeerDFS(network::Network*);
      void on_read(messages::Message*, int) override;
      void on_connect() override;
      void on_disconnect(int) override;
      virtual void update(int, std::string, std::string&, uint64_t, uint64_t);
      virtual void request(int, std::string, req_func);
      void close ();
      bool list(messages::FileList*);
      bool format();
      bool check_exist(std::string, std::string);
      bool insert_file(messages::FileInfo*);
      bool update_file(messages::FileUpdate*);
      bool delete_file(messages::FileDel*);
      FileDescription request_file(messages::FileRequest*);
      bool insert_block(messages::BlockInfo*);
      bool update_block(messages::BlockUpdate*);
      bool delete_block(messages::BlockDel*);
      NodeDescription request_node(messages::NodeRequest*);

    protected:
      void replicate_metadata();
      Directory directory;
      Local_io local_io;
      std::map<std::string, req_func> requested_blocks;
      template <typename T> void process (T);
      int NUM_NODES;
  };
}
