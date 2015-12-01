#include "noderemote.hh"
#include "../messages/factory.hh"
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

using boost::lexical_cast;
using boost::asio::ip::tcp;
using namespace boost::asio;
using namespace std;
using namespace std::placeholders;

namespace eclipse {
// constructor & destructor {{{
NodeRemote::NodeRemote(NodeLocal* n) : owner(*n), ioservice (n->io_service) { }

NodeRemote::NodeRemote(NodeLocal* n, int id, string host_, int p) :
   Node(id), owner(*n), ioservice (n->io_service), host(host_), port(p)
{
}
// }}}
// do_connect {{{
bool NodeRemote::do_connect() {
  tcp::resolver resolver (ioservice);
  tcp::resolver::query query (host, lexical_cast<string>(port));
  tcp::resolver::iterator endpoint_iterator (resolver.resolve(query));

  socket.reset (new tcp::socket (ioservice));
  boost::asio::async_connect (*socket, endpoint_iterator, 
      boost::bind(&NodeRemote::on_connect, this, 
        boost::asio::placeholders::error,
        boost::asio::placeholders::iterator));
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
