#pragma once

#include "../messages/message.hh"
#include "../common/context_singleton.hh"
#include <boost/asio.hpp>
#include <boost/asio/error.hpp>

namespace eclipse {
namespace network {

using boost::asio::ip::tcp;
const int header_size = 16;

class Channel {
  public:
    Channel ();
    ~Channel () = default;

    virtual void do_write (messages::Message*) = 0; 

  protected:
    boost::asio::io_service& iosvc;
    int port;
    Logger* logger = nullptr;
};

}
}
