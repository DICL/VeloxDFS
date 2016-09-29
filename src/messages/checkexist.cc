#include "checkexist.hh"

using namespace eclipse::messages;

CheckExist::CheckExist() {}

CheckExist::CheckExist(std::string name, std::string type) {
  this->name = name;
  this->type = type;
}

std::string CheckExist::get_type() const { return "CheckExist"; }
