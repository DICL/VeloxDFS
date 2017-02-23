#pragma once
#include "../messages/message.hh"
#include "netobserver.hh"
#include "server.hh"
#include <vector>

namespace eclipse {
namespace network {

using boost::asio::ip::tcp;
using vec_str = std::vector<std::string>;

class ClientHandler {
  public:
    ClientHandler(uint32_t port);
    ~ClientHandler() = default;

    bool send(uint32_t i, messages::Message* m);
    bool send_and_replicate(std::vector<int>, messages::Message*);

  private:
    bool send(uint32_t i, std::shared_ptr<std::string> str);
    void connect(uint32_t i, std::shared_ptr<Server> server);

    //! Only for internal network, IO_SERVER is owner of 
    //! it. It can be freed any time.
    std::map<uint32_t, std::weak_ptr<Server>> current_servers;

    vec_str nodes;
    NetObserver* node;
    uint32_t port;
};

}
} /* eclipse  */ 
