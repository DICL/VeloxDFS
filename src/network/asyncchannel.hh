#pragma once

#include "channel.hh"
#include "asyncnode.hh"
#include "../messages/message.hh"
#include <string>
#include <vector>
#include <boost/asio/spawn.hpp>

namespace eclipse {
namespace network {

class AsyncChannel: public Channel {
  public:
    AsyncChannel(Context&, int, AsyncNode*);

    void on_connect() override;
    void do_read () override;
    void do_write (messages::Message*) override; 

  protected:
    void on_write (const boost::system::error_code&, size_t, 
        Message*); 
    void read_coroutine (boost::asio::yield_context);
    AsyncNode* node;
};

}
}
