#include "boundaries.hh"

namespace network {

Boundaries::Boundaries (std::vector<uint64_t> v) : data(v) { }
std::string Boundaries::get_type() const { return "Boundaries"; }

}
