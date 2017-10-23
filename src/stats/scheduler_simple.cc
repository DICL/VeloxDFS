#include "logical_blocks_scheduler.hh"
#include "../messages/blockinfo.hh"
#include <algorithm>

#include <set>

using namespace std;
using namespace eclipse;
using namespace eclipse::messages;
using namespace eclipse::logical_blocks_schedulers;

void scheduler_simple::generate(FileDescription& file_desc, std::vector<std::string> nodes) {
  // Find nodes which contains chunks 
  set<string> hosts;
  copy(file_desc.block_hosts.begin(), file_desc.block_hosts.end(), inserter(hosts ,hosts.begin()));

  // Simple best fit
  int i = 0;
  for (auto host : hosts) {
    uint32_t which_node = find(nodes.begin(), nodes.end(), host) - nodes.begin();

    logical_block_metadata metadata;
    metadata.name = string("logical_") + file_desc.name + "_" + to_string(i++);
    metadata.file_name = file_desc.name;
    metadata.seq = i;
    metadata.hash_key = boundaries->random_within_boundaries(which_node);
    metadata.host_name = host;

    uint64_t size_of_chunks = 0;
    for (unsigned j = 0; j < file_desc.block_hosts.size(); j++) {
      if (file_desc.block_hosts[j] == host) {
        BlockInfo physical_block;
        physical_block.name      = file_desc.blocks[j];
        physical_block.file_name = file_desc.name;
        physical_block.hash_key  = file_desc.hash_keys[j];
        physical_block.size      = file_desc.block_size[j];
        physical_block.node      = host;
        
        size_of_chunks += physical_block.size; 
        metadata.physical_blocks.push_back(physical_block);
      }
    }

    metadata.size = size_of_chunks;

    file_desc.n_lblock = hosts.size();
    file_desc.logical_blocks.push_back(metadata);
  }

}
