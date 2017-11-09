#pragma once
#include "../messages/message.hh"
#include "netobserver.hh"
#include "server.hh"

#include <vector>
#include <boost/thread/shared_mutex.hpp>

namespace eclipse {
namespace network {

class ClientHandler {
  public:
    ClientHandler(uint32_t port);
    ~ClientHandler() = default;

    void attach(NetObserver*);
    bool send(uint32_t i, messages::Message* m);
    bool send_and_replicate(std::vector<int>, messages::Message*);

  private:
    bool try_reuse_client(uint32_t i, std::shared_ptr<std::string>);
    bool send(uint32_t i, std::shared_ptr<std::string> str);
    void connect(uint32_t i, std::shared_ptr<Server> server);

    //! Only for internal network, IO_SERVER is owner of 
    //! it. It can be freed any time.
    std::map<uint32_t, std::weak_ptr<Server>> current_servers;

    //! Reader/Writer lock for current_servers map
    boost::shared_mutex rw_lock;

    std::vector<std::string> nodes;
    NetObserver* local_router = nullptr;
    uint32_t port;
    uint32_t id;
};

}
} /* eclipse  */ 
