#include "topology.hh"
#include <string>
#include <vector>

namespace eclipse {
namespace network {

Topology::Topology (boost::asio::io_service& io,
    vec_str v, int p, int i) : 
   nodes(v), port(p), id(i), ioservice(io) {}
Channel* Topology::get_channel (int i) { 
  return channels[i].get(); 
}

} /* network  */ 
} /* eclipse  */ 
