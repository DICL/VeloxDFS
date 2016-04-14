#pragma once
#include "peerdfs.hh"
#include "router.hh"
#include "../messages/boost_impl.hh"

namespace eclipse {

using boost::system::error_code;
using boost::asio::ip::tcp;

class RemoteDFS: public Router {
  public:
    RemoteDFS ();
    ~RemoteDFS () = default;

    virtual bool establish ();
    void insert_block (messages::Message*, int);
    void insert_file (messages::Message*, int);
    void request_file (messages::Message*, int);
    void request_block (messages::Message*, int);
    void request_ls (messages::Message*, int);
    void delete_file (messages::Message*, int);
    void delete_block (messages::Message*, int);
    void send_block (std::string, std::string, int);
    void request_format (messages::Message*, int);
    void file_exist (messages::Message*, int);

  private:
    PeerDFS* peer_dfs = nullptr;
};

} /* eclipse  */ 
