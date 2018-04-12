// Variable length Multiple blocks scheduler
#include "logical_blocks_scheduler.hh"
#include "../messages/blockinfo.hh"
#include "../common/context_singleton.hh"

#include <algorithm>
#include <functional>
#include <set>

// Default 32MiB
#define MIN_BLOCK_SIZE (1 << 25) 

using namespace std;
using namespace eclipse;
using namespace eclipse::messages;
using namespace eclipse::logical_blocks_schedulers;

using VEC_STR = std::vector<std::string>;
using VEC_DOUBLE = std::vector<double>;
using VEC_INT = std::vector<int>;
using VEC_STATS = std::vector<std::pair<double,int>>;

// get_replicas_id {{{
vector<uint32_t> get_replicas_id(VEC_STR nodes, string node) {
  vector<uint32_t> output;
  int which_node = find(nodes.begin(), nodes.end(), node) - nodes.begin();
  output.push_back((which_node - 1 < 0) ? (nodes.size()-1) : (which_node - 1));
  output.push_back(which_node);
  output.push_back((which_node + 1 >= (int)nodes.size()) ? 0 : (which_node + 1));

  return output;
};
// }}}
// score {{{
double score(VEC_STATS& stats, VEC_DOUBLE& usage, double alpha, int id) {
  double w = alpha / 2.0;
  double cpu_percentage = 1.0 - double(double(stats[id].second) / 8.0f);
  double io_percentage = double(stats[id].first);

  return 1.00 - ((w*io_percentage) + (w*cpu_percentage) + ((1.00-alpha)*usage[id])) - double(stats[id].second == 0) ;
}
// }}}
// get_highest_id {{{
int get_highest_id(VEC_STATS& stats, VEC_DOUBLE& usage, VEC_STR hosts,
    string host, double alpha) {
  vector<double> score_vec;

  // For each chunk get its replicas
  auto replicas = get_replicas_id(hosts, host);

  // Then get each replicas' score
  std::transform(replicas.begin(), replicas.end(), back_inserter(score_vec),
      bind(&score, stats, usage, alpha, placeholders::_1));

  // Finally get the id of the highest replica's score
  auto highest_score_it = std::max_element(score_vec.begin(), score_vec.end());

  int replica_idx = distance(score_vec.begin(), highest_score_it);

  return replicas[replica_idx];
}
// }}}
// Generate {{{
void scheduler_vlmb::generate(FileDescription& file_desc, std::vector<std::string> nodes) {

  //---------------------INPUT DATA----------------------------------
  double alpha = atof(options["alpha"].c_str()); // IO factor
  auto stats = listener->get_io_stats();

  vector<double> usage;
  usage.resize(nodes.size(), .0);

  // HOST -> [L1, L2, L3...]
  std::map<int, std::vector<logical_block_metadata>> lblocks_dist; 
  
  //---------------------LOGIC STARTS HERE---------------------------

  // For each chunk find its logical block
  int logical_block_counter = 0;
  for (int i = 0; i < int(file_desc.blocks.size()); i++) {
    int smallest_id = get_highest_id(stats,
        usage, nodes, file_desc.block_hosts[i], alpha);

    // Iterators to key and to logical block
    auto node_it = lblocks_dist.find(smallest_id);
    vector<logical_block_metadata>::iterator current_lblock_it;

    // Create new node's lblocks key
    if (node_it == lblocks_dist.end()) {
      node_it = lblocks_dist.insert({smallest_id, {}}).first;
    }

    double current_cpus  = stats[smallest_id].second;
    int current_lblocks  = node_it->second.size();

    // No maximum logical blocks formed
    if (current_lblocks < current_cpus and current_cpus != 0) {

      // Add new logical block
      if (node_it->second.size() == 0 || node_it->second.back().size >= MIN_BLOCK_SIZE) {
        logical_block_metadata metadata;
        metadata.seq = logical_block_counter;
        metadata.name = string("logical_") + file_desc.name + "_" + to_string(logical_block_counter++);
        metadata.file_name = file_desc.name;
        metadata.hash_key = boundaries->random_within_boundaries(smallest_id);
        metadata.host_name = nodes[smallest_id];
        current_lblock_it = node_it->second.insert(node_it->second.end(), {metadata});

      // If last block size is small enough
      } else {
        current_lblock_it = node_it->second.end() - 1;
      }

    // No free cpus
    } else {
      current_lblock_it = min_element(node_it->second.begin(), node_it->second.end(), 
          [] (auto& a, auto& b) { return a.size < b.size; });
    }

    // FINALLY, insert the chunk in the logical block
    BlockInfo physical_block;
    physical_block.name      = file_desc.blocks[i];
    physical_block.file_name = file_desc.name;
    physical_block.hash_key  = file_desc.hash_keys[i];
    physical_block.size      = file_desc.block_size[i];
    physical_block.seq       = i;
    physical_block.node      = nodes[smallest_id]; //Choose this replica

    current_lblock_it->size += physical_block.size;
    current_lblock_it->physical_blocks.push_back(physical_block);

    usage[smallest_id] += double(1.0/double(file_desc.blocks.size()));
  }

  // Copy the block distribution to the file_descriptior
  for (auto& kv : lblocks_dist) {
    std::copy(kv.second.begin(), kv.second.end(), back_inserter(file_desc.logical_blocks));
  }

  int i = 0;
  for (auto us : usage) {
    INFO("USAGE %d : %lf", i++, us);
  }

  file_desc.n_lblock = file_desc.logical_blocks.size();
}
// }}}
