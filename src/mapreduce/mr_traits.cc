#include "mr_traits.hh"

using namespace std;
namespace eclipse {

using vec_str = std::vector<std::string>;

// Constructor {{{
MR_traits::MR_traits(Context& context) : 
  NodeLocal(context) {
  Settings& setted = context.settings;

  vec_str nodes = setted.get<vec_str> ("network.nodes");
  string master = setted.get<string> ("network.master");
  int port =      setted.get<int> ("network.port_mapreduce");
  id = find(nodes.begin(), nodes.end(), ip_of_this) - nodes.begin();

  auto net_ = new CentralizedTopology (this, io_service, 
      logger, master, nodes, port, id);
  network.reset(net_);
}

MR_traits::~MR_traits() { }
//}}}
// establish {{{
bool MR_traits::establish () {
  return network->establish();
}
// }}}
}
