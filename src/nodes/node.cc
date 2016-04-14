#include "node.hh"
#include "../common/settings.hh"

#include <vector>
#include <string>

using std::string;
using namespace std;
using vec_str    = std::vector<std::string>;

namespace eclipse {
// Constructor & destructor {{{
Node::Node() : 
  Machine(context.id),
  logger (context.logger.get()),
  port   (context.settings.get<int>("network.ports.internal"))
{
}

Node::~Node() { }
// }}}
// get_ip {{{
string Node::get_ip() const { return ip_of_this; }
// }}}
}
