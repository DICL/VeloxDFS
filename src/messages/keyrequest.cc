#include "keyrequest.hh"

namespace eclipse {
namespace messages {

KeyRequest::KeyRequest (std::string k) : key(k) { }

std::string KeyRequest::get_type() const { return "KeyRequest"; }

}
}
