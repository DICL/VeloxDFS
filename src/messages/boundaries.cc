#include "boundaries.hh"

namespace eclipse {
namespace messages {

Boundaries::Boundaries (std::vector<uint64_t> v) : data(v) { }
std::string Boundaries::get_type() const { return "Boundaries"; }

}
}
