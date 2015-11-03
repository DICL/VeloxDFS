#include "node.hh"

namespace Nodes {

Node::Node(int id_) : id(id_) { }
int Node::get_id() const { return id; }

}
