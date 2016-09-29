#include "mdlistlist.hh"

namespace eclipse {
namespace messages {

MdlistList::MdlistList (std::vector<MdlistInfo> v) : data(v) { }
std::string MdlistList::get_type() const { return "MdlistList"; }

}
}
