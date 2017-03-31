#include "filedescription.hh"

using namespace eclipse::messages;

FileDescription& FileDescription::operator=(FileDescription& rhs) {
  FileInfo::operator=(rhs);

  this->blocks.clear();
  this->hash_keys.clear();
  this->block_size.clear();
 
  this->blocks = rhs.blocks;
  this->hash_keys = rhs.hash_keys;
  this->block_size = rhs.block_size;

  return *this;
}

std::string FileDescription::get_type() const { return "FileDescription"; }
