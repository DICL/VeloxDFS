#include "asyncchannel.hh"

namespace eclipse {
namespace network {
// constructor {{{
AsyncChannel::AsyncChannel(Context& c, int i, AsyncNode* node_) : 
  Channel(c, i),
  node (node_),
  host(c.settings.get<std::vector<std::string>>("network.nodes")[i])
{ }
// }}}
}
} /* eclipse */
