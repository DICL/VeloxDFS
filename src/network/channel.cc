#include "channel.hh"

namespace eclipse {
namespace network {

Channel::Channel (Context& c, int id_) :
  iosvc  (c.io), 
  id     (id_),
  logger (c.logger.get()),
  send_socket (new tcp::socket(c.io)),
  recv_socket (new tcp::socket(c.io)) {}
 
void Channel::update_recv(tcp::socket* in ) {
  delete recv_socket;
  recv_socket = in;
  on_connect();
}

}
}
