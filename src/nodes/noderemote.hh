#pragma once

#include "node.hh"
#include "nodelocal.hh"
#include "../network/channel.hh"
#include "../messages/message.hh"
#include "../common/logger.hh"

#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/error.hpp>

namespace eclipse {
namespace network {
class Channel;
}

using boost::asio::ip::tcp;
using namespace messages;
using namespace boost::asio;
using namespace boost::system;
using namespace network;

class NodeLocal; // <- Forward initialization

class NodeRemote: public Node {
  public:
    NodeRemote (NodeLocal*);
    NodeRemote (NodeLocal*, int);
    ~NodeRemote () = default;

    std::string get_ip () const override;

    virtual void action () = 0;
    virtual void do_read () = 0;
    virtual void do_write (messages::Message*) = 0; 

    void set_channel (Channel*);

  protected:
    Channel* channel = nullptr;
    NodeLocal* owner = nullptr;
    io_service& ioservice;
    Logger* logger = nullptr;
};
}
