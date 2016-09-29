#include "noderequest.hh"

using namespace eclipse::messages;

NodeRequest::NodeRequest() {}

NodeRequest::NodeRequest(std::string name) {
  this->name = name;
}

std::string NodeRequest::get_type() const { return "NodeRequest"; }
