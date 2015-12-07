#include "channel.hh"

namespace eclipse {
namespace network {

Channel::Channel (boost::asio::io_service& io, string s, int p) : 
  endpoint(s), port(p), io_service(io) {}

tcp::socket* Channel::get_socket() { return socket.get(); }

}
}
