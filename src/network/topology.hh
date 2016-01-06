#pragma once

#include "channel.hh"
#include <string>
#include <vector>
#include <memory>

namespace eclipse {
namespace network {

using vec_str = std::vector<std::string>;

class Topology {
  public:
    Topology (boost::asio::io_service&, vec_str, int, int);
    virtual ~Topology () { };

    virtual bool establish () = 0;
    virtual bool close () = 0;

 //   Channel* get_channel (int);

  protected:
    vec_str nodes;
    int port, id;
    boost::asio::io_service& ioservice;
};

} /* network */ 
} /* eclipse */ 
