#include "channel.hh"

namespace eclipse {
namespace network {

Channel::Channel (tcp::socket* s): send_(s) {}

tcp::socket*  Channel::send_socket(){ return send_; }
tcp::socket*  Channel::recv_socket(){ return recv_; }
// set_recv_socket {{{
void Channel::set_recv_socket (tcp::socket* s ) {
  this->recv_ = s;
}
// }}}
}
}
