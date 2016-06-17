#include "offsetkv.hh"

namespace eclipse {
namespace messages {

OffsetKeyValue::OffsetKeyValue (uint32_t k, std::string n, std::string v, uint32_t p, uint32_t l) : key(k), name(n), value(v), pos(p), len(l) { }

std::string OffsetKeyValue::get_type() const { return "OffsetKeyValue"; }

}
}
