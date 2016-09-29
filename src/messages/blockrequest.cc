#include "blockrequest.hh"

using namespace eclipse::messages;

BlockRequest::BlockRequest() {}

BlockRequest::BlockRequest(std::string name, int net_id) {
  this->name = name;
  this->net_id = net_id;
}

std::string BlockRequest::get_type() const { return "BlockRequest"; }
