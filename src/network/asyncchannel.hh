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
    AsyncChannel(tcp::socket*, tcp::socket*, AsyncNode*);
    void do_write (messages::Message*) override; 
    void do_read ();

  protected:
    void on_write (const boost::system::error_code&, size_t, 
        messages::Message*, std::string*); 

    void read_coroutine (boost::asio::yield_context);

    AsyncNode* node = nullptr;
    tcp::socket *sender, *receiver;
};

}
}
