#pragma once

#include "node.hh"
#include "nodelocal.hh"
#include "../network/channel.hh"
#include "../messages/message.hh"

#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/error.hpp>

namespace eclipse {

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

    void close ();
    std::string get_ip () const override;

    virtual void on_connect (const error_code&, tcp::resolver::iterator) = 0;
    virtual void start () = 0;

  protected:
    Channel* channel;
    NodeLocal* owner;
    io_service& ioservice;
};
}
