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
    Server(Context&, AsyncNode*);

    void do_write (messages::Message*) override; 
    void on_accept (tcp::socket*);
    bool is_multiple() override;
    void do_read ();

  protected:
    void on_write (const boost::system::error_code&, size_t, 
        Message*); 

    void read_coroutine (boost::asio::yield_context);

    tcp::socket *server = nullptr;
};

}
}
