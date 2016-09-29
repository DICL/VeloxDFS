#include "blockupdate.hh"

using namespace eclipse::messages;

BlockUpdate::BlockUpdate () {}
BlockUpdate::BlockUpdate (std::string name, int net_id, uint64_t pos, uint64_t len, int replica, std::string content) {
  this->name = name;
  this->net_id = net_id;
  this->pos = pos;
  this->len = len;
  this->replica = replica;
  this->content = content;
}

std::string BlockUpdate::get_type() const { return "BlockUpdate"; }
