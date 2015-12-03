#pragma once

#include "noderemote.hh"
#include "nodelocal.hh"
#include "../messages/message.hh"
#include <string>
#include <vector>

namespace eclipse {

class PeerRemote: public NodeRemote {
  public:
    using NodeRemote::NodeRemote;

    std::vector<char> msg_inbound;

    void on_connect (const boost::system::error_code&,
        boost::asio::ip::tcp::resolver::iterator) override;
    void do_read ();
    void on_read (const boost::system::error_code&, size_t); 
    void do_write (messages::Message*); 
    void on_write (boost::system::error_code&, size_t); 

    void send (messages::Message*);
};

}
