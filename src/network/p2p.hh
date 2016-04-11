#pragma once

#include "channel.hh"
#include "asyncnode.hh"
#include "asyncchannel.hh"
#include "../messages/message.hh"
#include <string>
#include <vector>
#include <utility>
#include <boost/asio/spawn.hpp>
#include <sstream>

namespace eclipse {
namespace network {

// @pre server and client socket should be open and not null
class P2P: public AsyncChannel {
  public:
    P2P(tcp::socket*, tcp::socket*, Context&, AsyncNode*);
    void do_write (messages::Message*) override; 
    bool is_multiple() override;
    void do_read ();

  protected:
    void on_write (const boost::system::error_code&, size_t, 
        Message*, std::string*); 

    void read_coroutine (boost::asio::yield_context);
    tcp::socket *client, *server;
};

}
}
