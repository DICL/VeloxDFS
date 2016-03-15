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

class Server: public AsyncChannel {
  public:
    Server(Context&, int, AsyncNode*);

    void do_connect () override; 
    void do_write (messages::Message*) override; 
    void on_accept (tcp::socket*);

  protected:
    void on_write (const boost::system::error_code&, size_t, 
        Message*); 

    void do_read ();
    void read_coroutine (boost::asio::yield_context);

    tcp::socket *server = nullptr;
    tcp::endpoint* endpoint = nullptr;
};

}
}
