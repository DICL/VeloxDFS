#include "meshnetwork.hh"

namespace eclipse {
namespace network {
// Constructor {{{
MeshNetwork::MeshNetwork (AsyncNode* node, Context& c) {
  Settings& setted = c.settings;
  vec_str nodes  = setted.get<vec_str> ("network.nodes");

  int i = 0;
  for (auto& node_: nodes) {
    if (i != c.id) 
      channels.insert ({i, new AsyncChannel(c, i, node)});
    i++;
  }

  topology = new MeshTopology (c, &channels, node);
}

MeshNetwork::~MeshNetwork () {  }
// }}}
// establish {{{
bool MeshNetwork::establish () {
  topology->establish();
  return true;
}
// }}}
// close {{{
bool MeshNetwork::close () {
  return true;
}
// }}}
// size {{{
size_t MeshNetwork::size () {
  return channels.size();
}
// }}}
// send {{{
bool MeshNetwork::send (int i, messages::Message* m) {
  channels[i]->do_write(m);
  return true;
}
// }}}
}
}
