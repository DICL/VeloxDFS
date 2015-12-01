#pragma once

#include "noderemote.hh"
#include "../messages/message.hh"

namespace eclipse {

class PeerRemote: public NodeRemote {
  public:
    using NodeRemote::NodeRemote;
    ~PeerRemote () = default;

    void on_connect (boost::system::error_code&) override;
    void do_read ();
    void on_read (boost::system::error_code&, size_t); 
    void do_write (messages::Message*); 
    void on_write (boost::system::error_code&, size_t); 

    void send (messages::Message*);
};

}
