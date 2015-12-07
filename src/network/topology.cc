#include <string>
#include <vector>

namespace eclipse {
namespace network {

Topology::Topology (vec_str& v, int p) : nodes(v), port(p) {}
Channel* Topology::get_channel (int i) { 
  return channels[i].get(); 
}

} /* network  */ 
} /* eclipse  */ 
