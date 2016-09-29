#include "sliceinfo.hh"

using namespace eclipse::messages;

SliceInfo::SliceInfo() {}

SliceInfo::SliceInfo(std::string name, uint32_t hash_key, uint64_t size, std::string& content) {
  this->name = name;
  this->hash_key = hash_key;
  this->size = size;
  this->content = content;
}

SliceInfo::SliceInfo(std::string name, uint32_t hash_key, uint64_t size) {
  this->name = name;
  this->hash_key = hash_key;
  this->size = size;
  this->content = "";
}

std::string SliceInfo::get_type() const { return "SliceInfo"; }
