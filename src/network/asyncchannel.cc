#include "asyncchannel.hh"

namespace eclipse {
namespace network {
// constructor {{{
AsyncChannel::AsyncChannel(Context& c, AsyncNode* node_) : 
  Channel(c),
  node (node_)
{ }
// }}}
}
} /* eclipse */
