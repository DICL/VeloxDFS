#pragma once

#include "noderemote.hh"
#include "peerlocal.hh"
#include "../messages/message.hh"
#include <string>
#include <vector>

namespace eclipse {

class PeerRemote: public NodeRemote {
  protected:
    PeerLocal* owner_peer = nullptr;

    boost::asio::streambuf inbound_data;
    char inbound_header [16];
    const int header_size = 16;
    
  public:
    PeerRemote(NodeLocal*, int);

    void do_read () override;
    void do_write (messages::Message*) override; 

    void on_write (const boost::system::error_code&, size_t, 
        Message*); 
    void on_read_header (const boost::system::error_code&, size_t); 
    void on_read_body (const boost::system::error_code&, size_t); 

    void start () override;
};

}
