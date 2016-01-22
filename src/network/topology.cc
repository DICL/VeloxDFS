#include "topology.hh"
#include <string>
#include <vector>

namespace eclipse {
namespace network {

Topology::Topology (boost::asio::io_service& io, Logger* l,
    vec_str v, int p, int i) : 
   nodes(v), port(p), id(i), ioservice(io), logger(l) {}

Channel* Topology::get_channel (int i) { 
  return channels[i]; 
}

} /* network  */ 
} /* eclipse  */ 
