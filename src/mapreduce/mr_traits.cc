#include "mr_traits.hh"

namespace eclipse {

MR_traits::MR_traits(Settings& setted) : 
  NodeLocal(Settings& setted) {

  setted.load();

  vec_str nodes = setted.get<vec_str> ("network.nodes");
  string master =  setted.get<string> ("network.master");
  int port = setted.get<int> ("network.port_mapreduce");
}

MR_traits::~MR_traits() { }

}
// establish {{{
bool MR_traits::establish () {

  auto net_ = CentralizedTopology()
}
// }}}
