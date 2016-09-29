#include "nodelist.hh"

namespace eclipse {
namespace messages {

NodeList::NodeList (std::vector<NodeInfo> v) : data(v) { }
std::string NodeList::get_type() const { return "NodeList"; }

}
}
