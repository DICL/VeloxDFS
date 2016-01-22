#include "asio.hh"

namespace network {
// constructor & Destructor {{{
Asio::Asio (boost::asio::io_service& io) : ioservice(io) {

}
// }}}
// connect {{{
bool Asio::connect (std::string host, int port) {
  acceptor.reset(new tcp::acceptor (ioservice, tcp::endpoint (tcp::v4(), port)));.
  return true;
}
//}}}
// listen {{{
bool Asio::listen (int port) {
}
// }}}
// close {{{
bool Asio::close (Connection* conn) {
  return true;
}
//}}}
// send {{{
bool Asio::send (const Message* m) {
  return true;
}
//}}}
// recv {{{
bool Asio::recv (Message* m) {
  return true;
}
//}}}
}
