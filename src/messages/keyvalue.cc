#include "keyvalue.hh"

using namespace eclipse::messages;

KeyValue::KeyValue (int k, std::string n, std::string v) : key(k), name(n), value(v) { }

std::string KeyValue::get_type() const { return "KeyValue"; }
