#include "keyvalue.hh"

namespace eclipse {
namespace messages {

KeyValue::KeyValue (uint32_t k, std::string n, std::string v) : key(k), name(n), value(v) { }

std::string KeyValue::get_type() const { return "KeyValue"; }

}
}
