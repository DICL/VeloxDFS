#include "channel.hh"

namespace eclipse {
namespace network {

Channel::Channel (tcp::socket& s): socket(s) {}
tcp::socket* Channel::get_socket() { return &socket; }

}
}
