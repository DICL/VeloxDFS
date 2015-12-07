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
    Topology (vec_str&, int);
    virtual ~Topology () { };

    virtual bool establish () = 0;
    virtual bool close () = 0;

    Channel* get_channel (int);

  protected:
    vec_str nodes;
    int port;
    std::vector<std::unique_ptr<Channel> > channels;
};

} /* network */ 
} /* eclipse */ 
