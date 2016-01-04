#include "noderemote.hh"
#include "../messages/factory.hh"
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

using namespace std;
using namespace std::placeholders;
using boost::lexical_cast;
namespace ph = boost::asio::placeholders;

namespace eclipse {
// constructor & destructor {{{
NodeRemote::NodeRemote(NodeLocal* n) : owner(n), ioservice (n->io_service) { }

NodeRemote::NodeRemote(NodeLocal* n, int id) :
   Node(id), owner(n), ioservice (n->io_service)
{
}
// }}}
// close {{{
void NodeRemote::close() {
}
// }}}
// get_ip {{{
string NodeRemote::get_ip() const { return "placeholder"; }
// }}}
// set_channel {{{
void NodeRemote::set_channel (Channel* c) {
  channel = c;
}
// }}}
}
