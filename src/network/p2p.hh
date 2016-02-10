#pragma once

#include "channel.hh"
#include "asyncnode.hh"
#include "asyncchannel.hh"
#include "../messages/message.hh"
#include <string>
#include <vector>
#include <boost/asio/spawn.hpp>

namespace eclipse {
namespace network {

class P2P: public AsyncChannel {
  public:
    P2P(Context&, int, AsyncNode*);

    void do_connect () override; 
    void do_write (messages::Message*) override; 
    void on_accept (tcp::socket*);

  protected:
    void on_connect (const boost::system::error_code&);
    void on_write (const boost::system::error_code&, size_t, 
        Message*); 

    void do_read ();
    void read_coroutine (boost::asio::yield_context);

    tcp::socket client, *server;
    tcp::endpoint* endpoint = nullptr;

    bool client_connected = false, server_connected = false;
};

}
}
