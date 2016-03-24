#pragma once
#include "peerdfs.hh"
#include "router.hh"
#include "../messages/boost_impl.hh"

namespace eclipse {

using boost::system::error_code;
using boost::asio::ip::tcp;

class RemoteDFS: public Router {
  public:
    RemoteDFS (Context&);
    ~RemoteDFS () = default;

    bool establish ();
    void insert_block (messages::Message*);
    void insert_file (messages::Message*);
    void request_file (messages::Message*);
    void request_block (messages::Message*);
    void request_ls (messages::Message*);
    void delete_file (messages::Message*);
    void delete_block (messages::Message*);
    void send_block (std::string, std::string);
    void request_format (messages::Message*);
    void file_exist (messages::Message*);

  protected:
    PeerDFS peer;
};

} /* eclipse  */ 
