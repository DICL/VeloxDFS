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
NodeLocal::NodeLocal(Settings& setted) {
  setted.load();

  string logname = setted.get<string> ("log.name");
  string logtype = setted.get<string> ("log.type");
  ip_of_this     = setted.getip ();
  port           = setted.get<int> ("network.port_cache");

  logger.reset (Logger::connect(logname, logtype));

}

NodeLocal::~NodeLocal() { }
// }}}
// get_ip {{{
string NodeLocal::get_ip() const { return ip_of_this; }
// }}}
}
