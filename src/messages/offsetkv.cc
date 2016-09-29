#include "offsetkv.hh"

using namespace eclipse::messages;

OffsetKeyValue::OffsetKeyValue (int k, std::string n, std::string v, uint64_t p, uint64_t l) : key(k), name(n), value(v), pos(p), len(l) { }

std::string OffsetKeyValue::get_type() const { return "OffsetKeyValue"; }
