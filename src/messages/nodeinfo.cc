#include "nodeinfo.hh"

using namespace eclipse::messages;

NodeInfo::NodeInfo() {}

NodeInfo::NodeInfo(std::string name, std::string file_name, int id, int net_id) {
  this->name = name;
  this->file_name = file_name;
  this->id = id;
  this->net_id = net_id;
}

NodeInfo& NodeInfo::operator=(const NodeInfo& other) {
  this->name = other.name;
  this->file_name = other.file_name;
  this->id = other.id;
  this->net_id = other.net_id;
  return *this;
}

std::string NodeInfo::get_type() const { return "NodeInfo"; }
