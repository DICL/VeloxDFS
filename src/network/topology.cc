#include "topology.hh"
#include <string>
#include <vector>

namespace eclipse {
namespace network {

Topology::Topology (Context& c) : 
  nodes     (c.settings.get<vec_str> ("network.nodes")),
  port      (c.settings.get<int>("network.port_cache")),
  id        (c.id),
  ioservice (c.io),
  logger    (c.logger.get())
{ }

} /* network  */ 
} /* eclipse  */ 
