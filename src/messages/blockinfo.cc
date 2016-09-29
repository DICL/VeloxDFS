#include "blockinfo.hh"

using namespace eclipse::messages;

BlockInfo::BlockInfo() {}

BlockInfo::BlockInfo(std::string name, uint32_t hash_key, std::string file_name,
    uint64_t seq, uint64_t size, int net_id, int replica, int check_commit, std::string& content) {
  this->name = name;
  this->hash_key = hash_key;
  this->file_name = file_name;
  this->seq = seq;
  this->size = size;
  this->net_id = net_id;
  this->replica = replica;
  this->check_commit = check_commit;
  this->content = content;
}

BlockInfo::BlockInfo(std::string name, uint32_t hash_key, std::string file_name,
      uint64_t seq, uint64_t size, int net_id, int check_commit) {
  this->name = name;
  this->hash_key = hash_key;
  this->file_name = file_name;
  this->seq = seq;
  this->size = size;
  this->net_id = net_id;
  this->check_commit = check_commit;
}

std::string BlockInfo::get_type() const { return "BlockInfo"; }
