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
NodeRemote::NodeRemote(NodeLocal& n) : owner(n), ioservice (n.io_service) { }

NodeRemote::NodeRemote(NodeLocal& n, int id, string host_, int p) :
   Node(id), owner(n), ioservice (n.io_service), host(host_), port(p)
{
}
// }}}
// do_connect {{{
void NodeRemote::do_connect() {
  tcp::resolver resolver (ioservice);
  tcp::resolver::query query (host, lexical_cast<string>(port));
  tcp::resolver::iterator endpoint_iterator (resolver.resolve(query));

  socket.reset (new tcp::socket (ioservice));
  async_connect (*socket, endpoint_iterator, 
      bind(&NodeRemote::on_connect, this, ph::error, ph::iterator));
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
