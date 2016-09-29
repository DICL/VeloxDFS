#include "fileinfo.hh"

using namespace eclipse::messages;

FileInfo::FileInfo() {}

FileInfo::FileInfo(std::string name, uint32_t hash_key, uint64_t size, uint64_t num_block, std::string type, int replica) {
  this->name = name;
  this->hash_key = hash_key;
  this->size = size;
  this->num_block = num_block;
  this->type = type;
  this->replica = replica;
}

FileInfo& FileInfo::operator=(const FileInfo& other) {
  this->name = other.name;
  this->hash_key = other.hash_key;
  this->size = other.size;
  this->num_block = other.num_block;
  this->type = other.type;
  this->replica = other.replica;
  return *this;
}

std::string FileInfo::get_type() const { return "FileInfo"; }
