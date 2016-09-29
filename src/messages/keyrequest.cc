#include "keyrequest.hh"

using namespace eclipse::messages;

KeyRequest::KeyRequest (const std::string& k) : key(k) {}
std::string KeyRequest::get_type() const { return "KeyRequest"; }
