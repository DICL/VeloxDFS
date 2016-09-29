#include "boundaries.hh"

using namespace eclipse::messages;

Boundaries::Boundaries (std::vector<uint32_t> v) : data(v) { }
std::string Boundaries::get_type() const { return "Boundaries"; }
