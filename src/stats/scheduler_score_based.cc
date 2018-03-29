#include "logical_blocks_scheduler.hh"
#include "../messages/blockinfo.hh"

#include <algorithm>
#include <set>

using namespace std;
using namespace eclipse;
using namespace eclipse::messages;
using namespace eclipse::logical_blocks_schedulers;

void scheduler_score_based::generate(FileDescription& file_desc, std::vector<std::string> nodes) {
  double alpha = atof(options["alpha"].c_str());
  double beta  = atof(options["beta"].c_str());

  auto io_vec = listener->get_io_stats();

  // Find nodes which contains chunks 
  set<string> hosts;
  copy(file_desc.block_hosts.begin(), file_desc.block_hosts.end(), inserter(hosts ,hosts.begin()));

  {
    auto replica = [nodes] (std::string node) {
      vector<uint32_t> output;
      int which_node = find(nodes.begin(), nodes.end(), node) - nodes.begin();
      output.push_back((which_node - 1 < 0) ? (nodes.size()-1) : (which_node - 1)); 
      output.push_back(which_node);
      output.push_back((which_node + 1 >= (int)nodes.size()) ? 0 : (which_node + 1));

      for (int x : output)
        cout << x << endl;
      
      return output;
    };

    int j = 0;
    for (int i = 0; i < (int)file_desc.blocks.size(); i++) {

      double max_score = 0.00f;
      string host;

      for (int server_id : replica(file_desc.block_hosts[i])) {
        double score = (alpha * (1.00 - io_vec[server_id].first)) + (beta * (0.0));
        if (score >= max_score) {
          max_score = score;
          host = nodes[server_id];
        }
      }
      // Instert the block into logical block
      //
      auto& l_blocks = file_desc.logical_blocks;
      auto it = std::find_if(l_blocks.begin(), l_blocks.end(), [host] (auto& l_block) {
            return (l_block.host_name == host);
          });

      if (it == l_blocks.end()) {
        uint32_t which_node = find(nodes.begin(), nodes.end(), host) - nodes.begin();
        logical_block_metadata metadata;
        metadata.seq = j;
        metadata.name = string("logical_") + file_desc.name + "_" + to_string(j++);
        metadata.file_name = file_desc.name;
        metadata.hash_key = boundaries->random_within_boundaries(which_node);
        metadata.host_name = host;
        it = l_blocks.insert(it, metadata);
      }

      BlockInfo physical_block;
      physical_block.name      = file_desc.blocks[i];
      physical_block.file_name = file_desc.name;
      physical_block.hash_key  = file_desc.hash_keys[i];
      physical_block.size      = file_desc.block_size[i];
      physical_block.node      = host;

      it->size += physical_block.size; 
      it->physical_blocks.push_back(physical_block);
    }
    file_desc.n_lblock = file_desc.logical_blocks.size();
  }
}
