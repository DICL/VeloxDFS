#pragma once

#include "../messages/message.hh"
#include "../common/context.hh"

#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/error.hpp>

namespace eclipse {
namespace network {

using boost::asio::ip::tcp;
using namespace messages;
using namespace boost::asio;
using namespace boost::system;
using namespace network;

class Channel {
  public:
    Channel (Context&);
    ~Channel () = default;

    virtual void do_connect () = 0;
    virtual void do_write (messages::Message*) = 0; 
    virtual bool is_multiple() = 0;

    const int header_size = 16;

  protected:
    io_service& iosvc;
    int port;
    Logger* logger = nullptr;
};
}
}
