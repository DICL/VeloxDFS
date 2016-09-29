#include "filedel.hh"

using namespace eclipse::messages;

FileDel::FileDel() {}

FileDel::FileDel(std::string name) {
  this->name = name;
}

std::string FileDel::get_type() const { return "FileDel"; }
