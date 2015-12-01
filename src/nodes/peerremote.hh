#pragma once

#include "noderemote.hh"
#include "nodelocal.hh"
#include "../messages/message.hh"
#include <string>

namespace eclipse {

class PeerRemote: public NodeRemote {
  public:
    using NodeRemote::NodeRemote;
    PeerRemote(NodeLocal* n, int id, std::string host_, int p) :
    //~PeerRemote () = default;

    void on_connect (const boost::system::error_code&,
        boost::asio::ip::tcp::resolver::iterator ) override;
    void do_read ();
    void on_read (boost::system::error_code&, size_t); 
    void do_write (messages::Message*); 
    void on_write (boost::system::error_code&, size_t); 

    void send (messages::Message*);
};

}
