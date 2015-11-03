#include "keyvalue.hh"

namespace network {
  KeyValue::KeyValue (std::string k, std::string v) : key(k), value(v) { }

  std::string KeyValue::get_type() const { return "KeyValue"; }
}
