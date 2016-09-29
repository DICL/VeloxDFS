#include "blocklist.hh"

namespace eclipse {
namespace messages {

BlockList::BlockList (std::vector<BlockInfo> v) : data(v) { }
std::string BlockList::get_type() const { return "BlockList"; }

}
}
