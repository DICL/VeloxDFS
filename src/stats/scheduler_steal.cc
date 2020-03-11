// Variable length Multiple blocks scheduler
#include "logical_blocks_scheduler.hh"
#include "../messages/blockinfo.hh"
#include "../common/context_singleton.hh"

#include <algorithm>
#include <functional>
#include <set>

using namespace std;
using namespace eclipse;
using namespace eclipse::messages;
using namespace eclipse::logical_blocks_schedulers;

using VEC_STR = std::vector<std::string>;

namespace {
	// get_replicas_id {{{
	// It should be modifed! -> random replica
	vector<uint32_t> get_replicas_id(VEC_STR nodes, string node) {
		vector<uint32_t> output;
		int which_node = find(nodes.begin(), nodes.end(), node) - nodes.begin();
		output.push_back(which_node);
		output.push_back((which_node - 1 < 0) ? (nodes.size()-1) : (which_node - 1));
		output.push_back((which_node + 1 >= (int)nodes.size()) ? 0 : (which_node + 1));

		return output;
	};
	// }}}
}


// Generate {{{
void scheduler_steal::generate(FileDescription& file_desc, std::vector<std::string> nodes) {
	// HOST -> [L1, L2, L3...]
	std::map<int, std::vector<logical_block_metadata>> lblocks_dist; 

	int nChunks =  int(file_desc.blocks.size());
	int SLOTS = GET_INT("addons.cores");
	int min_slots = nChunks / (int)nodes.size();
	SLOTS = SLOTS > min_slots ? min_slots : SLOTS;

	// Intialize the logical blocks
	int logical_block_counter = 0;
	for (int i = 0; i < (int)nodes.size(); i++) {
		auto it = lblocks_dist.insert({i, {}}).first;
		for (int j = 0; j < SLOTS; j++) {
			logical_block_metadata metadata;
			metadata.seq = logical_block_counter;
			metadata.name = string("logical_") + file_desc.name + "_" + to_string(logical_block_counter++);
			metadata.file_name = file_desc.name;
			metadata.hash_key = boundaries->random_within_boundaries(i);
			metadata.host_name = nodes[i];
			it->second.push_back(metadata);
		}
	}

	// For each chunk find its logical block
	double input_split = 1.0;
	int cut     =  int(nChunks*input_split);
	file_desc.num_static_blocks = cut;

	vector<int> node_chunks_counter(nodes.size(), 0);

	for (int i = 0; i < nChunks; i++) {
		auto& host = file_desc.block_hosts[i];
		auto replicas = get_replicas_id(nodes, host);

		BlockInfo physical_block;
		physical_block.name      = file_desc.blocks[i];
		physical_block.primary_file    = file_desc.primary_files[i]; // added
		physical_block.file_name = file_desc.name;
		physical_block.hash_key  = file_desc.hash_keys[i];
		physical_block.size      = file_desc.block_size[i]; 
		physical_block.offset 	 = file_desc.offsets[i];  // added
		physical_block.foffset 	 = file_desc.offsets_in_file[i];  // added
		physical_block.primary_seq 	 = file_desc.primary_sequences[i];  // added
		physical_block.seq       = i;
		// physical_block.node      = nodes[replicas[1]]; //Choose this replica

		// Find good candidate
		int min_size = INT_MAX;
		int target_node_id = -1;
		for (int id : replicas) {
			if (node_chunks_counter[id] < min_size) {
				target_node_id = id;
				min_size = node_chunks_counter[id];
			}
		}

		int current_index = node_chunks_counter[target_node_id] % SLOTS;
		node_chunks_counter[target_node_id] += 1;
		auto& lblock = lblocks_dist[target_node_id][current_index];

		physical_block.node      =  lblock.host_name;

		lblock.size += physical_block.size;
		lblock.primary_chunk_num += 1;
		lblock.physical_blocks.push_back(physical_block);
	}

	std::map<int, std::vector<logical_block_metadata>>::iterator iter; 
	for(iter = lblocks_dist.begin(); iter != lblocks_dist.end() ; iter++){
		for(int i = 0; i<SLOTS; i++){
			auto& host = iter->second[i].host_name; 
			int which_node = find(nodes.begin(), nodes.end(), host) - nodes.begin();
			auto replicas = get_replicas_id(nodes, host);
			for(int id : replicas) {
				if( id != which_node){
					//iter->second[i].physical_blocks.insert(iter->second[i].physical_blocks.end(), lblocks_dist[id][i].physical_blocks.begin(), lblocks_dist[id][i].physical_blocks[lblocks_dist[id][i].primary_chunk_num]);
					iter->second[i].physical_blocks.insert(iter->second[i].physical_blocks.end(), lblocks_dist[id][i].physical_blocks.begin(), lblocks_dist[id][i].physical_blocks.begin()+lblocks_dist[id][i].primary_chunk_num);
				}
			}
			cout<< endl;
		}
	}

	for (auto& kv : lblocks_dist) {
		std::copy(kv.second.begin(), kv.second.end(), back_inserter(file_desc.logical_blocks));
	}

	INFO("Finished scheduling");

	file_desc.n_lblock = file_desc.logical_blocks.size();
}
// }}}
