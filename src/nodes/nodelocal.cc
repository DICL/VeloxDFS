#include "nodelocal.hh"
#include "peerremote.hh"
#include "../common/settings.hh"

#include <vector>
#include <string>

using std::string;
using namespace std;

namespace eclipse {

using vec_str    = std::vector<std::string>;
using vec_node   = std::vector<NodeRemote*>;
using PAIR       = std::pair<int, NodeRemote*>;

// Constructor & destructor {{{
NodeLocal::NodeLocal(Context& context) : 
  logger    (context.logger.get()),
  ip_of_this(context.settings.getip()),
  io_service(context.io),
  port      (context.settings.get<int>("network.port_cache"))
{
  //ip_of_this     = context.settings.getip ();
 // port           = context.settings.get<int> ("network.port_cache");
}

NodeLocal::~NodeLocal() { }
// }}}
// get_ip {{{
string NodeLocal::get_ip() const { return ip_of_this; }
// }}}
}
