#include "noderemote.hh"
#include <boost/lexical_cast.hpp>
#include "../messages/factory.hh"

using boost::lexical_cast;
using namespace boost::asio;
using namespace std;

namespace Nodes {
// constructor & destructor {{{
NodeRemote::NodeRemote(io_ptr& io, string host_, int p, int id) :
 Node(id), ioservice (io), host(host_), port(p)
{
}
// }}}
// connect {{{
bool NodeRemote::connect() {
  tcp::resolver resolver (ioservice);
  tcp::resolver::query query (host, lexical_cast<string>(port));
  tcp::resolver::iterator endpoint_iterator (resolver.resolve(query));

  socket.reset (new tcp::socket (ioservice));
  connect (*socket, endpoint_iterator);
  return true;
}
// }}}
// close {{{
void NodeRemote::close() {
  socket->close();
}
// }}}
// get_ip {{{
string NodeRemote::get_ip() const { return host; }
// }}}
}
