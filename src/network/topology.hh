#pragma once

#include "channel.hh"
#include "../common/logger.hh"
#include <string>
#include <vector>
#include <memory>

namespace eclipse {
namespace network {

using vec_str = std::vector<std::string>;
template <typename T> using u_ptr = std::unique_ptr<T>;

class Topology {
  public:
    Topology (boost::asio::io_service&, Logger*, vec_str, 
        int, int);
    virtual ~Topology () { };

    virtual bool establish () = 0;
    virtual bool close () = 0;
    virtual bool is_online() = 0;

    Channel* get_channel (int);

  protected:
    vec_str nodes;
    int port, id;
    boost::asio::io_service& ioservice;
    std::map<int, Channel*> channels;
    Logger* logger;
};

} /* network */ 
} /* eclipse */ 
