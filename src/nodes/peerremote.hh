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
    boost::asio::ip::tcp::socket* socket;
    
  public:
    PeerRemote(NodeLocal*, int);

    std::vector<char> msg_inbound;

    void on_connect (const boost::system::error_code&,
        boost::asio::ip::tcp::resolver::iterator) override;
    void do_read ();
    void on_read (const boost::system::error_code&, size_t); 
    void do_write (messages::Message*); 
    void on_write (const boost::system::error_code&, size_t); 

    void send (messages::Message*);
    void start () override;
};

}
