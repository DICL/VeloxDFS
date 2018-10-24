#include "logical_blocks_scheduler.hh"
#include "../messages/blockinfo.hh"
#include "../common/context_singleton.hh"
#include <algorithm>
#include <iterator>
#include <utility>
#include <iostream>
#include <map>
#include <math.h>

using namespace std;
using namespace eclipse;
using namespace eclipse::messages;
using namespace eclipse::logical_blocks_schedulers;
using LBLOCKS = std::vector<logical_block_metadata>;
using CHUNKS  = std::vector<int>;
using VEC_STR = std::vector<std::string>;
using FD = FileDescription;

namespace {

const uint32_t MIN_BLOCK_SIZE = GET_INT("addons.min_block_size"); //#33554432;
const uint32_t SLOTS = GET_INT("addons.cores");

// split_blocks {{{
std::pair<CHUNKS, CHUNKS> split_blocks(CHUNKS chunks) {
  const uint32_t len = chunks.size();
  auto C_1 = CHUNKS(chunks.begin(), chunks.begin() + int(len/2.0));
  auto C_2 = CHUNKS(chunks.begin() + int(len/2.0) + 1, chunks.end());
  return {C_1, C_2};
}

vector<uint32_t> get_replicas_id(VEC_STR nodes, string node) {
  const uint32_t NETWORK_SIZE = nodes.size();
  vector<uint32_t> output;

  int which_node = find(nodes.begin(), nodes.end(), node) - nodes.begin();

  uint32_t id = (which_node - 1 < 0) ? (NETWORK_SIZE-1) : (which_node - 1);
  for (uint32_t i = 0; i < SLOTS; i++) {
    output.push_back(id*SLOTS + i);
  }

  id = which_node;
  for (uint32_t i = 0; i < SLOTS; i++) {
    output.push_back(id*SLOTS + i);
  }

  id = (which_node + 1 >= (int)NETWORK_SIZE) ? 0 : (which_node + 1);
  for (uint32_t i = 0; i < SLOTS; i++) {
    output.push_back(id*SLOTS + i);
  }

  return output;
};
// }}}
//  assign_chunks_to_slots {{{
void assign_chunks_to_slots(CHUNKS chunks, FD& fd, std::vector<std::string> nodes) {

  const uint32_t N_SLOTS = (SLOTS*nodes.size());
  map<uint32_t, vector<uint32_t>> slots_dist;

  for(uint32_t i = 0; i < N_SLOTS; i++) { 
    slots_dist[i] = vector<uint32_t>();
  }
  std::cout << "SIZE " << slots_dist.size() << std::endl;

  // Equally fill every bucket
  for (uint32_t i = 0; i < chunks.size(); i++) {
    auto slots_replicas = get_replicas_id(nodes, fd.block_hosts[i]);
    vector<uint32_t> replicas_size;

    transform(slots_replicas.begin(), slots_replicas.end(), back_inserter(replicas_size), [&slots_dist] (auto r) {
          return slots_dist[r].size();
        });

    auto replicas_idx = min_element(replicas_size.begin(), replicas_size.end()) - replicas_size.begin();
    auto core_id = slots_replicas[replicas_idx];

    //std::cout << "replica_idx " << replicas_idx << std::endl;
    //std::cout << "core_id " << core_id << std::endl;

    slots_dist[core_id].push_back(i);
  }

  std::cout << "SIZE " << slots_dist.size() << std::endl;

  // Append to the FD
  uint32_t lblock_idx = fd.n_lblock;
  for (auto& kv : slots_dist) {
    const int core_id = kv.first;
    const int node_id = floor(double(core_id)/SLOTS);
    const string host = nodes[node_id];
    auto& chunks_vec  = kv.second;

    logical_block_metadata metadata;
    metadata.name = string("logical_") + fd.name + "_" + to_string(lblock_idx);
    metadata.file_name = fd.name;
    metadata.seq = lblock_idx;
    metadata.hash_key = GET_INDEX_IN_BOUNDARY(node_id);
    metadata.host_name = host;

    lblock_idx++;

    uint64_t size_of_chunks = 0;
    for (auto index : chunks_vec) {
      BlockInfo physical_block;
      physical_block.name      = fd.blocks[index];
      physical_block.file_name = fd.name;
      physical_block.hash_key  = fd.hash_keys[index];
      physical_block.size      = fd.block_size[index];
      physical_block.node      = host;

      size_of_chunks += physical_block.size; 
      metadata.physical_blocks.push_back(physical_block);
    }

    metadata.size = size_of_chunks;

    fd.logical_blocks.push_back(metadata);
  }
  fd.n_lblock += N_SLOTS;
}
// }}}
// schedule {{{
bool schedule(CHUNKS chunks, FD& fd, std::vector<std::string> nodes) {
  const uint32_t CHUNK_SIZE = fd.intended_block_size;
  const uint32_t N_SLOTS = SLOTS*nodes.size();
  if (double(chunks.size())/N_SLOTS * CHUNK_SIZE < MIN_BLOCK_SIZE)
    return false;

  CHUNKS C_1, C_2;

  tie(C_1, C_2) = split_blocks(chunks);

  if (!schedule(C_2, fd, nodes))
    copy(C_2.begin(), C_2.end(), back_inserter(C_1));

  cout << "One more iteration" << endl;
  assign_chunks_to_slots(C_1, fd, nodes);

  return true;
}
// }}}
}
// generate {{{
void scheduler_multiwave::generate(FD& fd, VEC_STR nodes) {

  // Fancy for vec <- {0,1,2,3,4.. size(blocks)}
  CHUNKS chunks_id(fd.blocks.size());
  generate_n(chunks_id.begin(), fd.blocks.size(), []() { static uint32_t val = 0; return val++; });
  fd.n_lblock = 0;

  schedule(chunks_id, fd, nodes);
}
// }}}
