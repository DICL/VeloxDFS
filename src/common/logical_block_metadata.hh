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
  uint64_t hash_key = 0L; //
  uint64_t seq = 0L;
  int replica_chunk_num[2];
  int primary_chunk_num = 0; // the number of my chunk in Stealing scheduler
  
  std::vector<messages::BlockInfo> physical_blocks;
  inline uint32_t get_number_of_phy_blocks() { return physical_blocks.size(); }
};

}
