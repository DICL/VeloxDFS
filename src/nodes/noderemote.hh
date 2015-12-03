#pragma once

#include "node.hh"
#include "nodelocal.hh"
#include "../messages/message.hh"

#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/error.hpp>

namespace eclipse {

using boost::asio::ip::tcp;
using namespace messages;
using namespace boost::asio;
using namespace boost::system;

class NodeLocal; // <- Forward initialization

class NodeRemote: public Node {
  public:
    NodeRemote (NodeLocal&);
    NodeRemote (NodeLocal&, int, std::string, int);
    ~NodeRemote () = default;

    void do_connect ();
    void close ();
    std::string get_ip() const override;

    virtual void on_connect (const error_code&, tcp::resolver::iterator) = 0;

  protected:
    NodeLocal& owner;
    io_service& ioservice;

    u_ptr<tcp::socket> socket;
    std::string host;
    int port;
};
}
