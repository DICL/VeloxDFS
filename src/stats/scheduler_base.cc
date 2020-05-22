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
void scheduler_base::generate(FileDescription& file_desc, std::vector<std::string> nodes) {
	// HOST -> [L1, L2, L3...]
	std::map<int, logical_block_metadata> lblocks_dist; 

	int nChunks =  int(file_desc.blocks.size());

	// Intialize the logical blocks
	int logical_block_counter = 0;
	for (int i = 0; i < (int)nodes.size(); i++) {
		logical_block_metadata metadata;
		metadata.seq = logical_block_counter;
		metadata.name = string("logical_") + file_desc.name + "_" + to_string(logical_block_counter++);
		metadata.file_name = file_desc.name;
		metadata.hash_key = boundaries->random_within_boundaries(i);
		metadata.host_name = nodes[i];
		lblocks_dist.insert({i, metadata});
	}

	// For each chunk find its logical block
	file_desc.num_static_blocks = nChunks;

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

		int target_node_id = replicas[0];	// primary node 
		auto& lblock = lblocks_dist[target_node_id];

		physical_block.node  =  lblock.host_name;

		lblock.size += physical_block.size;
		lblock.primary_chunk_num += 1;
		lblock.physical_blocks.push_back(physical_block);
	}

	std::map<int, logical_block_metadata>::iterator iter; 

	for(iter = lblocks_dist.begin(); iter != lblocks_dist.end() ; iter++){
		auto replicas = get_replicas_id(nodes, iter->second.host_name);
		int which_node = iter->first;

		for(int id : replicas) {
			int idx = which_node == (id + 1) % nodes.size() ? 0 : 1; // we assume that the number of replica is 3
			iter->second.replica_chunk_num[idx] = lblocks_dist[id].primary_chunk_num;
			if( id != which_node){
				vector<messages::BlockInfo>::reverse_iterator riter_begin = lblocks_dist[id].physical_blocks.rend() - lblocks_dist[id].primary_chunk_num;
				vector<messages::BlockInfo>::reverse_iterator riter_end = lblocks_dist[id].physical_blocks.rend(); // + lblocks_dist[id].primary_chunk_num;
				iter->second.physical_blocks.insert(iter->second.physical_blocks.end(), riter_begin, riter_end);
			}
		}
	}


	std::map<int, logical_block_metadata>::iterator lblock;
	for(lblock = lblocks_dist.begin(); lblock != lblocks_dist.end(); lblock++ ){
		file_desc.logical_blocks.push_back(lblock->second);
	}
	
	INFO("Finished scheduling");

	file_desc.n_lblock = file_desc.logical_blocks.size();
}
// }}}
