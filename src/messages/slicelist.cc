#include "slicelist.hh"

namespace eclipse {
namespace messages {

SliceList::SliceList (std::vector<SliceInfo> v) : data(v) { }
std::string SliceList::get_type() const { return "SliceList"; }

}
}
