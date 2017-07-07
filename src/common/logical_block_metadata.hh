#pragma once
#include "../messages/blockinfo.hh"
#include <vector>
#include <string>

namespace eclipse {

struct logical_block_metadata {
  std::string file_name;
  std::string name;
  std::string host_name;

  uint64_t size     = 0L;
  uint64_t hash_key = 0L;
  uint64_t seq = 0L;

  std::vector<messages::BlockInfo> physical_blocks;
  inline uint32_t get_number_of_phy_blocks() { return physical_blocks.size(); }
};

}
