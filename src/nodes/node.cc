#include "node.hh"
#include "../common/settings.hh"

#include <vector>
#include <string>

using std::string;
using namespace std;
using vec_str    = std::vector<std::string>;

namespace eclipse {
// Constructor & destructor {{{
Node::Node(Context& context) : 
  Machine(context.id),
  logger    (context.logger.get()),
  settings (context.settings),
  ip_of_this(context.settings.getip()),
  io_service(context.io),
  port      (context.settings.get<int>("network.port_cache"))
{
}

Node::~Node() { }
// }}}
// get_ip {{{
string Node::get_ip() const { return ip_of_this; }
// }}}
}
