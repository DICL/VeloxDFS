#include "nodelocal.hh"
#include "../common/settings.hh"

#include <string>

using std::string;
using namespace std;

namespace Nodes {
// Constructor & destructor {{{
NodeLocal::NodeLocal() :
  logger (nullptr, Logger::disconnect)
{
  Settings setted = Settings().load();

  string logname = setted.get<string> ("log.name");
  string logtype = setted.get<string> ("log.type");
  ip_of_this     = setted.getip ();
  
  logger.reset (Logger::connect(logname, logtype));
}

NodeLocal::~NodeLocal() { }
// }}}
// get_ip {{{
string NodeLocal::get_ip() const { return ip_of_this; }
// }}}
}
