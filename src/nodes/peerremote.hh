#pragma once

#include "noderemote.hh"
#include "peerlocal.hh"
#include "../messages/message.hh"
#include <string>
#include <vector>
#include <boost/asio/spawn.hpp>

namespace eclipse {

class PeerRemote: public NodeRemote {
  public:
    PeerRemote(NodeLocal*, int);

    void action () override;
    void do_read () override;
    void do_write (messages::Message*) override; 

  protected:
    void on_write (const boost::system::error_code&, size_t, 
        Message*); 
    void read_coroutine (boost::asio::yield_context);
};

}
