#include "noderemote.hh"
#include "../messages/factory.hh"
#include <boost/lexical_cast.hpp>

using boost::lexical_cast;
using boost::asio::ip::tcp;
using namespace boost::asio;
using namespace std;
using namespace std::placeholders;

namespace eclipse {
// constructor & destructor {{{
NodeRemote::NodeRemote(NodeLocal* n) : owner(*n), ioservice (n->io_service) { }

NodeRemote::NodeRemote(NodeLocal* n, int id, string host_, int p) :
 owner(*n), ioservice (n->io_service), host(host_), port(p), Node(id)
{
}
// }}}
// do_connect {{{
bool NodeRemote::do_connect() {
  tcp::resolver resolver (ioservice);
  tcp::resolver::query query (host, lexical_cast<string>(port));
  tcp::resolver::iterator endpoint_iterator (resolver.resolve(query));

  socket.reset (new tcp::socket (ioservice, endpoint_iterator));
  boost::asio::async_connect (*socket, bind(&NodeRemote::on_connect, this, _1));
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
