#include "blockdel.hh"

using namespace eclipse::messages;

BlockDel::BlockDel() {}

BlockDel::BlockDel(std::string name) {
  this->name = name;
}

std::string BlockDel::get_type() const { return "BlockDel"; }
