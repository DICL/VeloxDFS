#pragma once
#include "channel.hh"
#include "asyncnode.hh"
#include "../messages/message.hh"
#include "netobserver.hh"
#include <string>
#include <vector>
#include <boost/asio/spawn.hpp>

namespace eclipse {
namespace network {

class AsyncChannel: public Channel {
  public:
    AsyncChannel(tcp::socket*, tcp::socket*, NetObserver*, int);
    ~AsyncChannel();
    void do_write (messages::Message*) override; 
    void do_write_impl (std::string*); 
    void do_read ();

  protected:
    void on_write (const boost::system::error_code&, size_t, std::string*); 

    void read_coroutine (boost::asio::yield_context);

    NetObserver* node = nullptr;
    tcp::socket *sender, *receiver;
    int id;
};

}
}
