// Headers {{{
#include "dfs.hh"
#include "../messages/boost_impl.hh"
#include "../messages/fileinfo.hh"
#include "../messages/factory.hh"
#include "../messages/fileinfo.hh"
#include "../messages/fileexist.hh"
#include "../messages/filedescription.hh"
#include "../messages/filerequest.hh"
#include "../messages/filelist.hh"
#include "../messages/reply.hh"
#include "../messages/blockrequest.hh"
#include "../common/context.hh"
#include "../common/hash.hh"
#include "../common/histogram.hh"
#include "../common/block.hh"
#include "../common/blockmetadata.hh"
#include "../common/shared_memory.hh"
#include "../messages/factory.hh"
#include "../messages/IOoperation.hh"
#include "../messages/task_operation.hh"
#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <fcntl.h>
#include <ext/stdio_filebuf.h>
#include <algorithm>
#include <stack>
#include <future>
#include <sys/resource.h>
#include <ctime>
#include <chrono>
#include <ctime>
#include <cstdio>
#include <unistd.h> 
//#include <zookeeper/zookeeper.h>

using namespace std;
using namespace eclipse;
using namespace boost::archive;
using namespace eclipse::messages;
using boost::asio::ip::tcp;
// }}}

namespace velox {

	std::map<std::string, std::shared_ptr<FileDescription>> file_description_cache;
	std::map<std::string, std::shared_ptr<ifstream> > primary_fd;

	double total_read_time = 0.0;
	uint64_t BLOCK_SIZE;
	int processed_chunk;
	struct semaphore * sema;

	//std::map<std::string, ifstream* > primary_fd;
	// Static functions {{{
	static unique_ptr<tcp::socket> connect(uint32_t hash_value) { 
		auto nodes = GET_VEC_STR("network.nodes");
		auto port = GET_INT("network.ports.client");
		string host;
		auto socket = make_unique<tcp::socket>(context.io);
		try {

			host = nodes[hash_value % nodes.size()];
			tcp::resolver resolver(context.io);
			tcp::resolver::query query(host, to_string(port));
			tcp::resolver::iterator it(resolver.resolve(query));
			auto ep = make_unique<tcp::endpoint>(*it);
			socket->connect(*ep);
		} catch (...) {
			cout << "host:"  << host << " port:" << port << endl;
		}
		return socket;
	}

	static unique_ptr<tcp::socket> connect_host(string host) { 
		auto nodes = GET_VEC_STR("network.nodes");
		auto port = GET_INT("network.ports.client");
		auto socket = make_unique<tcp::socket>(context.io);
		try {

			tcp::resolver resolver(context.io);
			tcp::resolver::query query(host, to_string(port));
			tcp::resolver::iterator it(resolver.resolve(query));
			auto ep = make_unique<tcp::endpoint>(*it);
			socket->connect(*ep);
		} catch (...) {
			cout << "host:"  << host << " port:" << port << endl;
		}
		return socket;
	}
	shared_ptr<FileDescription> get_file_description
		(std::function<unique_ptr<tcp::socket>(uint32_t)> connect, std::string& fname, bool logical_block = false) {

			string cache_key = fname;

			if (file_description_cache.find(cache_key) != file_description_cache.end()) {
				return (*file_description_cache.find(cache_key)).second;
			}

			uint32_t file_hash_key = h(fname);
			auto socket = connect(file_hash_key);

			FileExist fe;
			fe.name = fname;
			send_message(socket.get(), &fe);
			auto rep = read_reply<Reply> (socket.get());

			if (rep->message != "TRUE") {
				cerr << "[ERR] " << fname << " doesn't exist." << endl;
				return nullptr;
			}

			FileRequest fr;
			fr.name = fname;
			if (logical_block) {
				fr.type = "LOGICAL_BLOCKS";
				fr.generate = false;
			}

			send_message(socket.get(), &fr);
			shared_ptr<FileDescription> fd = (read_reply<FileDescription> (socket.get()));
			socket->close();

			file_description_cache.insert({fname, fd});
			return fd;
		}

	static bool file_exists_local(std::string filename) {
		ifstream ifile(filename);
		return ifile.good();
	} 
	// }}} 
	// read_from_disk {{{

	void read_from_disk(char* buf, BlockInfo chunk, uint64_t* read_bytes, uint64_t cursor, uint64_t length) {
		struct timespec start, end;

		clock_gettime(CLOCK_REALTIME, &start);
		string disk_path = GET_STR("path.scratch");
		string file_path = disk_path + string("/") + chunk.name;

		std::shared_ptr<ifstream> ifs;
		auto iter = primary_fd.find(file_path);

		if(iter == primary_fd.end()){ //first open

			std::shared_ptr<ifstream> tmp_ifs(new ifstream);
			tmp_ifs->open(file_path, ios::in | ios::binary);
			if (!tmp_ifs->good()) {
				ERROR("DFS CLIENT: Error opening local file %s", file_path.c_str()); 
				return;
			}

			primary_fd.insert({file_path, tmp_ifs});
			ifs = (primary_fd.find(file_path))->second;

		} else {
			ifs = iter->second;

			if(!ifs->good()){
				cout<<"DFS CLIENT: Error opening local file " << file_path << " from ifstream" << endl;
			}
		}

		ifs->seekg(cursor, ios::beg);
		ifs->read(buf + *read_bytes, length);

		if (ifs->gcount() != long(length))
			ERROR("Missing bytes in the chunk %s [cur:%ld r:%ld to_read: %ld]", chunk.name.c_str(), cursor, ifs->gcount(), length);

		clock_gettime(CLOCK_REALTIME, &end);
		double seconds = (end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000;
		total_read_time += seconds;	
		cout << "Total Read Time: " << total_read_time << endl;

		*read_bytes += length;
	}

	// }}} 
	// read_from_remote {{{

	void read_from_remote(char* buf, BlockInfo chunk, uint64_t* read_bytes, uint64_t cursor, uint64_t length, int which_node) {
		using namespace std::chrono;
		INFO("CLIENT: REMOTE-READ block: %s cursor:%ld len:%ld read_bytes:%ld", 
				chunk.name.c_str(), cursor, length, *read_bytes);
		IOoperation io_ops;
		io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_REQUEST;
		io_ops.block.first = chunk.name;
		io_ops.pos = cursor;
		io_ops.length = length;
		//	cout<<"Read_from_remote : "<< chunk.name << endl;
		auto beg_clock = high_resolution_clock::now();

		auto slave_socket = connect(which_node);
		auto connect_clock = high_resolution_clock::now();
		send_message(slave_socket.get(), &io_ops);
		auto msg = read_reply<IOoperation>(slave_socket.get());
		auto recv_clock = high_resolution_clock::now();
		auto r_len = msg->block.second.length();
		memcpy(buf + *read_bytes, msg->block.second.c_str(), size_t(r_len));
		slave_socket->close();
		auto end_clock = high_resolution_clock::now();

		auto time_elapsed = duration_cast<microseconds>(end_clock - beg_clock).count();
		auto time_connect = duration_cast<microseconds>(connect_clock - beg_clock).count();
		auto time_recv    = duration_cast<microseconds>(recv_clock - connect_clock).count();
		auto time_cp      = duration_cast<microseconds>(end_clock - recv_clock).count();

		INFO("CLIENT: REMOTE-READ block: %s cursor:%ld len:%ld read_bytes:%ld time:%lf r_len:%ld",
				chunk.name.c_str(), cursor, length, *read_bytes, time_elapsed, r_len);

		INFO("TIME TT:%ld CON:%ld RX:%ld CP:%ld", time_elapsed, time_connect, time_recv, time_cp);

		*read_bytes += r_len;
	}

	// }}} 
	// read_physical {{{
	uint64_t read_physical(std::string& file_name, char* buf, uint64_t off, uint64_t len, 
			FileDescription* fd) {

		auto nodes = GET_VEC_STR("network.nodes");

		off = std::max(0ul, std::min(off, fd->size));
		if (off >= fd->size) return 0;

		// Find where is the block
		int block_beg_seq = 0, block_end_seq = 0;
		uint64_t current_block_offset = 0;

		{
			uint64_t total_size = 0;
			for (int i = 0; i < (int)fd->block_size.size(); i++) {
				total_size += fd->block_size[i];

				if (total_size > off) {
					block_beg_seq = i;
					current_block_offset = total_size - fd->block_size[i];
					break;
				}
			}
		}

		{
			uint64_t total_size = 0;
			for (int i = 0; i < (int)fd->block_size.size(); i++) {
				total_size += fd->block_size[i];

				if (total_size >= (off+len-1)) {
					block_end_seq = i;
					break;
				}

				if (i == ((int)fd->block_size.size() - 1))
					block_end_seq = i;
			}
		}

		uint64_t remain_len = len;
		uint64_t read_bytes = 0;

		auto is_replica = [](int a, int b, int size) {
			if (a == b) 
				return true;

			if (b - 1 == a or b + 1 == a)
				return true;

			if (b == 0 and a == size-1)
				return true;

			if (b == size-1 and a == 0)
				return true;

			return false;
		};

		// Request blocks
		for (int i = block_beg_seq; i <= block_end_seq; i++) {

			uint32_t hash_key = fd->hash_keys[i];
			int which_node = GET_INDEX(hash_key);
			uint64_t cursor = (i == block_beg_seq && fd->block_size[i] > 0) ? (off-current_block_offset) : 0;
			uint64_t length = std::min((fd->block_size[i] - cursor), remain_len);

			//INFO("DFS CLIENT: READ [ID:%d|FN:%s|NODE:%i]", context.id, fd->blocks[i].c_str(), which_node);
			if (is_replica(which_node, context.id, nodes.size())) {

				//INFO("DFS CLIENT: LOCAL READ HIT");
				string disk_path = GET_STR("path.scratch");
				string file_path = disk_path + string("/") + fd->blocks[i];

				ifstream ifs (file_path, ios::in | ios::binary | ios::ate);
				if (!ifs.good()) {
					INFO("DFS CLIENT: Error opening local file %s", file_path.c_str()); 
					break;
				}

				ifs.seekg(cursor, ios::beg);
				ifs.read(buf + read_bytes, length);
				ifs.close();

			} else {
				IOoperation io_ops;
				io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_REQUEST;
				io_ops.block.first = fd->blocks[i];
				io_ops.pos = cursor;
				io_ops.length = length;

				auto slave_socket = connect(which_node);
				send_message(slave_socket.get(), &io_ops);
				auto msg = read_reply<IOoperation>(slave_socket.get());
				memcpy(buf + read_bytes, msg->block.second.c_str(), (size_t)msg->block.second.length());
				slave_socket->close();
			}

			remain_len -= length;
			read_bytes += length;

			if (remain_len <= 0) break;
		}

		if (read_bytes != len) {
			INFO("read_bytes: %lu != len: %lu (off:%lu, f:%s r:%lu, cu:%lu) [%lu, %lu]", 
					read_bytes, len, off, file_name.c_str(), remain_len, current_block_offset,
					block_beg_seq, block_end_seq);
		}

		return read_bytes;
	}
	// }}}
	// read_logical {{{
	// 1. Check if the off belongs to this node
	// 2. Find begining chunk
	// 3. Find ending chunk
	// 4. Read chunk
	//
	uint64_t read_logical(std::string& file_name, char* buf, uint64_t off, 
			uint64_t len, FileDescription* fd) {

		using namespace std;
		cout<< "Read_Logical : " << file_name << endl;
		// Nothing to read
		if (fd->logical_blocks.size() == 0) { return 0; }

		// --------------- INITIALIZE STUFF ------------------
		auto nodes = GET_VEC_STR("network.nodes");
		int lblock_beg_seq = 0;
		uint64_t current_lblock_offset = 0;
		off = std::max(0ul, std::min(off, fd->size));

		// --------------- COMPUTING OFFSETS ------------------

		// COMPUTE current_lblock_offset offset
		{
			uint64_t total_size = 0;
			for (int i = 0; i < int(fd->logical_blocks.size()); i++) {
				total_size += fd->logical_blocks[i].size;

				if (total_size > off) {
					lblock_beg_seq = i;
					break;
				}
				current_lblock_offset += uint64_t(fd->logical_blocks[i].size);
			}
		}

		string beg_host = fd->logical_blocks[lblock_beg_seq].host_name;

		bool is_local_node = bool(beg_host == nodes[context.id]);

		auto chunks = fd->logical_blocks[lblock_beg_seq].physical_blocks;

		int chunk_beg_seq = 0, chunk_end_seq = 0;
		uint64_t current_chunk_offset = 0;
		uint64_t relative_offset = off - current_lblock_offset;

		// COMPUTE current_chunk_offset and starting chunk
		{
			uint64_t total_size = 0;
			for (int i = 0; i < int(chunks.size()); i++) {
				total_size += chunks[i].size;

				if (total_size > relative_offset) {
					chunk_beg_seq = i;
					current_chunk_offset = total_size - chunks[i].size;
					break;
				}
			}
		}

		// COMPUTING ENDING CHUNK
		{
			uint64_t total_size = 0;
			for (int i = 0; i < int(chunks.size()); i++) {
				total_size += chunks[i].size;

				if (total_size >= (relative_offset+len)) {
					chunk_end_seq = i;
					break;
				}

				if (i == int(chunks.size() - 1))
					chunk_end_seq = i;
			}
		}

		// --------------- READ BLOCK ------------------

		DEBUG("Current_chunk_offset %ld [%i,%i] off %lu len %lu", 
				current_chunk_offset, chunk_beg_seq, chunk_end_seq, off, len);

		uint64_t remain_len = len;
		uint64_t read_bytes = 0;
		/*
		   for (int i = chunk_beg_seq; i <= chunk_end_seq && remain_len > 0; i++) {
		   auto chunk = chunks[i];

		   uint64_t cursor = (i == chunk_beg_seq && chunk.size > 0) ? (relative_offset - current_chunk_offset) : 0;
		   uint64_t length = std::min((chunk.size - cursor), remain_len);

		   DEBUG("CLIENT: LOCAL READ block: %s cursor:%ld len:%ld read_bytes:%ld", 
		   chunk.name.c_str(), cursor, length, read_bytes);

		   if (is_local_node) {
		   read_from_disk(buf, chunks[i], &read_bytes, cursor, length);

		// It will perform remote read
		} else {
		int which_node = GET_INDEX(chunks[i].hash_key);
		read_from_remote(buf, chunks[i], &read_bytes, cursor, length, which_node);
		}

		remain_len -= read_bytes;

		if (remain_len <= 0) break;
		}
		 */
		/**/
		/*which_node, vector< off, len >*/
		std::map<int, std::vector<pair<uint64_t, uint64_t> > > blockBatch;
		std::map<int, pair<std::string, uint64_t> > EachBlockTotalLen;
		std::vector<pair<uint64_t, uint64_t> >local_blocks_info;

		if(is_local_node){
			for (int i = chunk_beg_seq; i <= chunk_end_seq; i++) {
				local_blocks_info.push_back({chunks[i].offset, chunks[i].size});

			}
		}	else{		

			for (int i = chunk_beg_seq; i <= chunk_end_seq; i++) {
				uint64_t cursor = (i == chunk_beg_seq && chunks[i].size > 0) ? (relative_offset - current_chunk_offset) : 0;
				uint64_t length = std::min((chunks[i].size - cursor), remain_len);

				int which_node = GET_INDEX(chunks[i].hash_key);
				if(blockBatch.find(which_node) == blockBatch.end()){ //first
					blockBatch.insert({which_node, {}});
					EachBlockTotalLen.insert({which_node, {chunks[i].primary_file, 0}});
				}

				blockBatch[which_node].push_back({chunks[i].offset, chunks[i].size});
				EachBlockTotalLen[which_node].second += length;
			}
		}

		if(is_local_node){
			std::vector<pair<uint64_t, uint64_t> >::iterator iter;
			string disk_path = GET_STR("path.scratch");
			string file_path = disk_path + string("/") + chunks[0].primary_file;
			ifstream ifs (file_path, ios::in | ios::binary | ios::ate);

			if (!ifs.good()) {
				ERROR("DFS CLIENT: Error opening local file %s", file_path.c_str()); 
				//return;
			}

			for(iter = local_blocks_info.begin(); iter != local_blocks_info.end(); iter++){
				uint64_t cursor = (iter == local_blocks_info.begin() && iter->second> 0) ? (relative_offset - current_chunk_offset) : 0;
				uint64_t length = std::min((iter->second - cursor), remain_len);
				ifs.seekg(iter->first, ios::beg);
				ifs.read(buf + read_bytes, length);

				if (ifs.gcount() != long(length))
					ERROR("Missing bytes in the chunk %s [cur:%ld r:%ld to_read: %ld]", file_path, cursor, ifs.gcount(), length);

				read_bytes += length;		
				remain_len -= read_bytes;

				if (remain_len <= 0) break;

			}
		} else {
			std::map<int, std::vector< pair<uint64_t, uint64_t> > >::iterator iter;
			for( iter = blockBatch.begin() ; iter != blockBatch.end(); iter++){
				//read_from_remote(buf, EachBlockTotalLen[iter->first].first, &read_bytes, )
				//			read_logical_from_remote(buf, EachBlockTotalLen[iter->first].first, &read_bytes, EachBlockTotalLen[iter->first].second, iter->first, iter->second);
			}
		}

		DEBUG("DFSCLIENT READLOGICAL, LOCAL=%d  READ_BYTES: %lu CS: %u Current_chunk_offset %ld L %ld [%i,%i] off %lu len %lu", is_local_node, read_bytes, chunks.size(), current_chunk_offset, current_lblock_offset, chunk_beg_seq, chunk_end_seq, off, len);

		return read_bytes;
	}


	// }}}
	// Constructors and misc {{{
	DFS::DFS(string _mr_job_id, int _tmg_id, bool _initializer) { 
		NUM_NODES = context.settings.get<vector<string>>("network.nodes").size();
		replica = context.settings.get<int>("filesystem.replica");
		nodes = context.settings.get<vector<string>>("network.nodes");
		initializer = _initializer;
		BLOCK_SIZE = context.settings.get<int>("filesystem.block");

		cores = context.settings.get<int>("addons.cores"); // GET_INT("addons.cores");
		shm_buf_depth = GET_INT("addons.shm_buf_depth");
		shm_buf_width = GET_INT("addons.shm_buf_width");
		shm_buf_idx = 0;
		total_shm_buf_num = shm_buf_width * shm_buf_depth;

		struct rlimit core_limits;
		core_limits.rlim_cur = core_limits.rlim_max = RLIM_INFINITY;
		setrlimit(RLIMIT_CORE, &core_limits);

		tmg_id = _tmg_id;
		mr_job_id = _mr_job_id;

		if(!initializer){
			uint64_t total_buf_size = sizeof(bool) + (sizeof(struct shm_buf) + BLOCK_SIZE) * shm_buf_depth * shm_buf_width;
			shmid = shmget((key_t)(tmg_id + DEFAULT_KEY), total_buf_size, 0666);
			if(shmid == -1) {
				perror("shmget failed");
				exit(-1);	// exit failure
			}

			shared_memory = shmat(shmid, NULL, 0);
			if(shared_memory == (void*)-1) {
				perror("shmat attach is failed\n");
				exit(-1);
			}
			
			string sema_path = "/tmp/semaphore" + to_string(tmg_id);
			sema = semaphore_open(sema_path.c_str());

			shm_status_addr = (uint64_t)shared_memory;	
			shm_idx_addr = (uint64_t)shared_memory + sizeof(bool);
			shm_base_chunk_addr = shm_idx_addr + sizeof(struct shm_buf) * shm_buf_width * shm_buf_depth;
			processed_chunk = 0;

			to_del_buf = NULL;
		}

	}

	DFS::~DFS(){
		if(!initializer)
			shmdt(shared_memory);
	}

	// }}}
	// upload {{{
	int DFS::upload(std::string file_name, bool is_binary, uint64_t block_size) {
		uint64_t BLOCK_SIZE = block_size;
		if (block_size == 0) {
			BLOCK_SIZE = context.settings.get<int>("filesystem.block");
		}

		int replica = GET_INT("filesystem.replica");
		bool is_equal_sized_blocks = (GET_STR("filesystem.equal_sized_blocks") == "true");

		if (is_binary) {
			replica = NUM_NODES;
		}

		uint32_t file_hash_key = h(file_name);
		if (not file_exists_local(file_name)) {
			cerr << "[ERR] " << file_name << " cannot be found in your machine." << endl;
			return EXIT_FAILURE;
		}

		//! Does the file exists
		//
		/* file_info.uploading == 0 이어야 exists를반환하게 해야 함*/
		if (this->exists(file_name)) {
			cerr << "[ERR] " << file_name << " already exists in VeloxDFS." << endl;
			return EXIT_FAILURE;
		}

		//! Insert the file
		int fd = open(file_name.c_str(), 0);

		__gnu_cxx::stdio_filebuf<char> filebuf(fd, std::ios::in | std::ios::binary);
		istream myfile(&filebuf);

		FileInfo file_info;
		file_info.name = file_name;
		file_info.hash_key = file_hash_key;
		file_info.replica = replica;
		myfile.seekg(0, ios_base::end);
		file_info.size = myfile.tellg();
		file_info.is_input = true;
		file_info.uploading = 1;
		file_info.intended_block_size = BLOCK_SIZE;

		//! Send file to be submitted;
		auto socket = connect(file_hash_key);
		send_message(socket.get(), &file_info);

		//! Get information of where to send the file
		auto description = read_reply<FileDescription>(socket.get());
		socket->close();

		uint64_t start = 0;
		uint64_t end = start + BLOCK_SIZE;
		uint64_t current_block_size = 0;
		unsigned int block_seq = 0;

		//! Insert the blocks
		uint32_t i = 0;

		//vector<BlockMetadata> blocks_metadata;
		Block blocks[NUM_NODES]; 
		std::map<int, BlockMetadata> blocks_metadata;	
		std::map<int, BlockMetadata>::iterator iter;
		
		
		vector<future<bool>> slave_sockets;
		unsigned int Primary_seq = 0;
		
		uint32_t total_chunk_num = file_info.size % BLOCK_SIZE + 1;
		vector<uint32_t> chunks_per_node(NUM_NODES, 0);
		vector<bool> create_slab(NUM_NODES, true);
		while(total_chunk_num > 0){
			chunks_per_node[i]++;
			i = (i+1) % NUM_NODES;
			total_chunk_num--;
		}
		i = 0;	

		uint32_t cli_wbuf_sz = 128 * BLOCK_SIZE;
		uint64_t cli_wbuf_offset = 0;
		uint64_t cli_wbuf_read_sz = 0;
		vector<char> chunk(cli_wbuf_sz, 0);
		while (true) {
			if (end < file_info.size) {
				myfile.seekg(start+BLOCK_SIZE, ios_base::beg);

				if (is_equal_sized_blocks == false) {
					while (myfile.peek() != '\n') {
						myfile.seekg(-1, ios_base::cur);
						end--;
					}
				}
			} else {
				end = file_info.size;
			}

			BlockMetadata metadata;
			Block block;
			ChunkMetadata Chunk;
			IOoperation io_ops;

			bool is_first_block = bool((i == 0) && !is_equal_sized_blocks);
			current_block_size = end - start + bool(is_first_block);
			myfile.seekg(start, myfile.beg);

			if (is_first_block) {
				chunk[0] = '\n';
			}

			myfile.read(chunk.data() + is_first_block + cli_wbuf_offset, current_block_size - bool(is_first_block));
			cli_wbuf_offset += current_block_size;
			cli_wbuf_read_sz += current_block_size;
			posix_fadvise(fd, end, current_block_size, POSIX_FADV_WILLNEED);

			//! Load block metadata info
			int which_server = ((file_hash_key % NUM_NODES) + i) % NUM_NODES;
			chunks_per_node[which_server]--;
			iter = blocks_metadata.find(which_server);
			if(iter == blocks_metadata.end()){ //first block 
				blocks_metadata.insert({which_server, metadata});

				block.first = blocks_metadata[which_server].name = string("Primary_") + to_string(which_server) + string("_") + file_name;
				blocks_metadata[which_server].file_name = file_name;
				blocks_metadata[which_server].hash_key = GET_INDEX_IN_BOUNDARY(which_server);
				blocks_metadata[which_server].seq = Primary_seq++;
				blocks_metadata[which_server].size = current_block_size;
				blocks_metadata[which_server].replica = replica;
				blocks_metadata[which_server].node = nodes[which_server];
				blocks_metadata[which_server].l_node = nodes[(which_server-1+NUM_NODES)%NUM_NODES];
				blocks_metadata[which_server].r_node = nodes[(which_server+1+NUM_NODES)%NUM_NODES];
				blocks_metadata[which_server].is_committed = 1;

				Chunk.offset = 0; 
				Chunk.foffset = start; 
				Chunk.size = current_block_size;
				Chunk.chunk_seq = block_seq;
				Chunk.primary_seq = blocks_metadata[which_server].seq;
				Chunk.name = file_name + string("_") + to_string(block_seq);
				Chunk.primary_file = blocks_metadata[which_server].name;

				blocks_metadata[which_server].chunks.push_back(Chunk);
				iter = blocks_metadata.find(which_server);
			} else{
				Chunk.offset = iter->second.size;
				Chunk.foffset = start;
				Chunk.size = current_block_size;
				Chunk.chunk_seq = block_seq;
				Chunk.primary_seq = iter->second.seq;
				Chunk.name = file_name + string("_") + to_string(block_seq);
				Chunk.primary_file = iter->second.name;

				block.first = iter->second.name;
				iter->second.chunks.push_back(Chunk);
				iter->second.size += current_block_size;
			}
			
			if(cli_wbuf_sz <= cli_wbuf_offset || end >= file_info.size 
					|| chunks_per_node[which_server] <= 0){  // If slab is changed or meet eof, then send to block leader
				
				block.second.reserve(cli_wbuf_sz);
				block.second = move(chunk.data());
			
				if(create_slab[which_server]){
					io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_INSERT;
					io_ops.block = move(block);
					create_slab[which_server] = false;
				} else {
					io_ops.pos = cli_wbuf_offset - cli_wbuf_read_sz;
					io_ops.length = cli_wbuf_read_sz;
					io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_UPDATE;
					io_ops.block = move(block);
				}

				auto socket = connect(GET_INDEX(iter->second.hash_key));
				send_message(socket.get(), &io_ops);

				auto future = async(launch::async, [](unique_ptr<tcp::socket> socket) -> bool {

						auto reply = read_reply<Reply> (socket.get());
						socket->close();

						if (reply->message != "TRUE") {
						cerr << "[ERR] Failed to upload block . Details: " << reply->details << endl;
						return false;
						}

						return true;
						}, move(socket));

				slave_sockets.push_back(move(future));

				cli_wbuf_read_sz = 0;	
				memset(chunk.data(), 0, cli_wbuf_sz);
				cout << "Send Write Buffer : " << cli_wbuf_read_sz << " , " << cli_wbuf_offset << endl;
			}
			cout << "IT " << block_seq << "START: " << start << " END: " << end << " bs " << current_block_size << endl;
			if (end >= file_info.size) {
				break;
			}
			start = end;
			end = start + BLOCK_SIZE;
			block_seq++;
			if(chunks_per_node[which_server] <= 0){
				cli_wbuf_offset = 0;
				i++;
			}
		}

		std::vector<BlockMetadata> blocksMetadata;
		for(iter = blocks_metadata.begin() ; iter != blocks_metadata.end(); iter++){
			blocksMetadata.push_back(iter->second);
		}

		for (auto& future: slave_sockets)
			future.get();

		file_info.num_primary_file = Primary_seq;
		file_info.num_block = block_seq;
		file_info.blocks_metadata = blocksMetadata;
		file_info.uploading = 0;

		socket = connect(file_hash_key);
		send_message(socket.get(), &file_info);
		auto reply = read_reply<Reply> (socket.get());

		if (reply->message != "TRUE") {
			cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
			return EXIT_FAILURE;
		}

		socket->close();
		close(fd);

		cout << "[INFO] " << file_name << " is uploaded." << endl;
		cout << "[INFO] file size : " << file_info.size << endl;
		return EXIT_SUCCESS;
	}



	// }}}
	// download {{{
	int DFS::download(std::string file_name) {
		struct timespec start, end;
		double read_time = 0.0;

		//! Does the file exists
		if (not this->exists(file_name)) {
			cerr << "[ERR] " << file_name << " already doesn't exists in VeloxDFS." << endl;
			return EXIT_FAILURE;
		}

		uint32_t file_hash_key = h(file_name);
		auto socket = connect (file_hash_key);

		FileRequest fr;
		fr.name = file_name;

		send_message(socket.get(), &fr);
		auto fd = read_reply<FileDescription> (socket.get());
		socket->close();

		ofstream file;
		file.rdbuf()->pubsetbuf(0, 0);      //! No buffer
		file.open(file_name, ios::binary);

		for (uint32_t i = 0; i < fd->blocks.size(); i++) {
			clock_gettime(CLOCK_REALTIME, &start);
			IOoperation io_ops;
			io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_REQUEST;
			io_ops.block.first = fd->primary_files[i];
			io_ops.pos = fd->offsets[i];
			io_ops.length = fd->block_size[i];

			cout<<"Block : "<< fd->blocks[i]  <<" Hash : "<<fd->hash_keys[i]<< " Length: "<<io_ops.length << endl;
			auto slave_socket = connect(GET_INDEX(fd->hash_keys[i]));
			send_message(slave_socket.get(), &io_ops);
			auto msg = read_reply<IOoperation>(slave_socket.get());

			clock_gettime(CLOCK_REALTIME, &end);
			read_time += (end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec ) / 1000000000; 

			file.write(msg->block.second.c_str(), msg->block.second.length());
			slave_socket->close();
		}


		file.close();
		cout << "Read Time : " << read_time << endl;
		return EXIT_SUCCESS;
	}
	// }}}
	// read_all {{{
	std::string DFS::read_all(std::string file) {

		auto fd = get_file_description(
				std::bind(&connect, std::placeholders::_1), file
				);
		if(fd == nullptr) return "";

		std::string output;
		int index = 0;

		for (auto block_name : fd->blocks) {
			IOoperation io_ops;
			io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_REQUEST;
			io_ops.block.first = fd->blocks[index];

			auto slave_socket = connect(GET_INDEX(fd->hash_keys[index]));
			send_message(slave_socket.get(), &io_ops);
			auto msg = read_reply<IOoperation>(slave_socket.get());
			output += msg->block.second;
			slave_socket->close();
			index++;
		}

		return output;
	}
	// }}} 
	// remove {{{
	int DFS::remove(std::string file_name) {

		uint32_t file_hash_key = h(file_name);
		auto socket = connect(file_hash_key);
		FileRequest fr;
		fr.name = file_name;

		send_message(socket.get(), &fr);
		auto fd = read_reply<FileDescription>(socket.get());

		unsigned int block_seq = 0;
		for (auto block_name : fd->primary_files) {
			uint32_t block_hash_key = fd->hash_keys[block_seq++];
			auto tmp_socket = connect(GET_INDEX(block_hash_key));
			IOoperation io_ops;
			io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_DELETE;
			io_ops.block.first = block_name;

			send_message(tmp_socket.get(), &io_ops);
			auto msg = read_reply<Reply>(tmp_socket.get());
			if (msg->message != "TRUE") {
				cerr << "[ERR] " << block_name << "doesn't exist." << endl;
				return EXIT_FAILURE;
			}
		}

		FileDel file_del;
		file_del.name = file_name;
		//socket = connect(file_hash_key);
		send_message(socket.get(), &file_del);
		auto reply = read_reply<Reply>(socket.get());
		if (reply->message != "OK") {
			cerr << "[ERR] " << file_name << " doesn't exist." << endl;
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}
	// }}}
	// rename {{{
	bool DFS::rename(std::string src, std::string dst) {

		auto src_socket = connect(h(src));
		FileRequest fr;
		fr.name = src;
		send_message(src_socket.get(), &fr);
		auto fd = read_reply<FileDescription>(src_socket.get());

		if(fd->uploading) {
			cout << "uploading :  " << fd->uploading << endl;

			src_socket->close();
			return false;
		}

		FileInfo file_info;
		file_info.name = dst;
		file_info.hash_key = h(dst);
		file_info.replica = fd->replica;
		file_info.size = fd->size;
		file_info.num_block = fd->num_block;
		file_info.num_primary_file = fd->num_primary_file;
		file_info.n_lblock = fd->n_lblock;
		file_info.is_input = fd->is_input;

		auto dst_socket = connect(file_info.hash_key);
		send_message(dst_socket.get(), &file_info);

		read_reply<FileDescription>(dst_socket.get());

		BlockMetadata metadata[NUM_NODES] = {0};

		for (int i = 0; i < (int) fd->num_block; i++) {
			int current_node = fd->hash_keys[i]%NUM_NODES;
			if(metadata[current_node].hash_key == 0){ //first
				metadata[current_node].hash_key = fd->hash_keys[i];
				metadata[current_node].file_name = dst;
				metadata[current_node].replica = fd->replica;
				metadata[current_node].node = fd->block_hosts[i];
				metadata[current_node].seq = fd->primary_sequences[i];
				metadata[current_node].size = 0;

				int which_server = 0;
				for(int j = 0; j < (int) NUM_NODES; ++j) {
					/* which_server = the node with file data */
					if(nodes[j] == fd->block_hosts[i]) {
						which_server = j;
						break;
					}
				}
				metadata[current_node].l_node = nodes[(which_server-1+NUM_NODES)%NUM_NODES];
				metadata[current_node].r_node = nodes[(which_server+1+NUM_NODES)%NUM_NODES];

				//file_info.blocks_metadata.push_back(metadata);
			} 

			ChunkMetadata smetadata;
			smetadata.size = fd->block_size[i];
			smetadata.offset = fd->offsets[i];
			smetadata.foffset = fd->offsets_in_file[i];
			smetadata.chunk_seq = fd->chunk_sequences[i];
			smetadata.primary_seq = fd->primary_sequences[i];

			metadata[current_node].size += smetadata.size;
			metadata[current_node].chunks.push_back(smetadata);

		}

		for(int i = 0; i<NUM_NODES; i++){
			if(metadata[i].hash_key == 0) // 
				continue;
			metadata[i].is_committed = 1;
			file_info.blocks_metadata.push_back(metadata[i]);	
		}

		file_info.uploading = 0;

		send_message(dst_socket.get(), &file_info);

		auto reply = read_reply<Reply>(dst_socket.get());

		bool ret = false;
		if(reply->message == "TRUE") {
			FileDel file_del;
			file_del.name = src;
			send_message(src_socket.get(), &file_del);
			auto reply = read_reply<Reply>(src_socket.get());
			ret = true;
		}
		else {
			cout << "failed rename" << endl;
		} 

		dst_socket->close();
		src_socket->close();

		return ret;
	}
	// }}}
	// format {{{
	int DFS::format() {
		for (unsigned int net_id = 0; net_id < NUM_NODES; net_id++) {
			FormatRequest fr;
			auto socket = connect(net_id);
			send_message(socket.get(), &fr);
			auto reply = read_reply<Reply>(socket.get());

			if (reply->message != "OK") {
				cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
				return EXIT_FAILURE;
			} 
		}
		cout << "[INFO] dfs format is done." << endl;
		return EXIT_SUCCESS;
	}
	// }}}
	// append {{{
	//! @todo fix implementation
	int DFS::append(string file_name, string buf) {
		string ori_file_name = file_name; 

		uint32_t file_hash_key = h(ori_file_name);
		auto socket = connect(file_hash_key);
		FileExist fe;
		fe.name = ori_file_name;
		send_message(socket.get(), &fe);
		auto rep = read_reply<Reply> (socket.get());

		if (rep->message != "TRUE") { // exist check
			cerr << "[ERR] " << ori_file_name << " doesn't exist." << endl;
			return EXIT_FAILURE;
		}
		FileRequest fr;
		fr.name = ori_file_name;

		istringstream myfile (buf);
		//ifstream myfile(new_file_name);
		myfile.seekg(0, myfile.end);
		uint64_t new_file_size = myfile.tellg();
		if (new_file_size <= 0) { // input size check
			cerr << "[ERR] " << buf << " size should be greater than 0." << endl;
			return EXIT_FAILURE;
		}

		// start normal append procedure
		send_message(socket.get(), &fr);
		auto fd = read_reply<FileDescription> (socket.get());
		uint64_t BLOCK_SIZE = fd->intended_block_size;

		int block_seq = fd->blocks.size()-1; // last block
		uint32_t ori_start_pos = 0; // original file's start position (in last block)
		uint64_t to_write_byte = new_file_size;
		uint64_t write_byte_cnt = 0;
		bool update_block = true; // 'false' for append
		bool new_block = false;
		uint32_t hash_key = fd->hash_keys[block_seq];
		uint64_t write_length = 0;
		uint64_t start = 0;
		uint64_t end = 0;
		uint32_t block_size = 0;
		vector<BlockMetadata> blocks_metadata;

		while (to_write_byte > 0) { // repeat until to_write_byte == 0
			BlockMetadata metadata;
			Block block;
			IOoperation io_ops;

			if (update_block == true) { 
				ori_start_pos = fd->block_size[block_seq];
				if (BLOCK_SIZE - ori_start_pos > to_write_byte) { // can append within original block
					myfile.seekg(start + to_write_byte, myfile.beg);
				} else { // can't write whole bufs in one block
					myfile.seekg(start + BLOCK_SIZE - ori_start_pos - 1, myfile.beg);
					new_block = true;
					while (1) {
						if (myfile.peek() =='\n' || myfile.tellg() == 0) {
							break;
						} else {
							myfile.seekg(-1, myfile.cur);
						}
					}
					if (myfile.tellg() <= 0) {
						update_block = false;
					}
				}
			}
			if (update_block == true) { // update block
				write_length = myfile.tellg();
				write_length -= start;
				myfile.seekg(start, myfile.beg);
				char *buffer = new char[write_length+1];
				//bzero(buffer, write_length+1);
				memset(buffer, 0, write_length+1);
				myfile.read(buffer, write_length);
				string sbuffer(buffer);
				delete[] buffer;

				int which_server = (hash_key  % NUM_NODES);

				metadata.name = fd->blocks[block_seq];
				metadata.file_name = ori_file_name;
				metadata.seq = block_seq;
				metadata.replica = fd->replica;
				metadata.hash_key = hash_key;
				metadata.size = ori_start_pos + write_length;
				metadata.node = nodes[which_server];
				metadata.l_node = nodes[(which_server-1+NUM_NODES)%NUM_NODES];
				metadata.r_node = nodes[(which_server+1+NUM_NODES)%NUM_NODES];
				metadata.is_committed = 1;

				blocks_metadata.push_back(metadata);

				block.first = metadata.name;
				block.second = move(sbuffer);

				io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_UPDATE;
				io_ops.block = move(block);
				io_ops.pos = ori_start_pos;
				io_ops.length = write_length;

				auto block_server = connect(GET_INDEX(metadata.hash_key));
				send_message(block_server.get(), &io_ops);
				auto reply = read_reply<Reply> (block_server.get());
				block_server->close();
				if (reply->message != "TRUE") {
					cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
					return EXIT_FAILURE;
				} 

				// calculate total write bytes and remaining write bytes
				to_write_byte -= write_length;
				write_byte_cnt += write_length;
				start += write_length;
				if (new_block == true) { 
					update_block = false;
				}
			} else { // append block
				// make new block
				block_seq++;
				int which_server = ((file_hash_key % NUM_NODES) + block_seq) % NUM_NODES;
				start = write_byte_cnt;
				end = start + BLOCK_SIZE -1;

				if (end < to_write_byte) {
					// not final block
					myfile.seekg(end, myfile.beg);
					while (1) {
						if (myfile.peek() =='\n') {
							break;
						} else {
							myfile.seekg(-1, myfile.cur);
							end--;
						}
					}
				} else {
					end = start + to_write_byte;
				}
				myfile.seekg(start, myfile.beg);
				block_size = (uint32_t) end - start;
				write_length = block_size;
				char *buffer = new char[block_size+1];
				bzero(buffer, block_size+1);
				myfile.read(buffer, block_size);
				string sbuffer(buffer);
				delete[] buffer;
				myfile.seekg(start, myfile.beg);

				metadata.name = ori_file_name + "_" + to_string(block_seq);
				metadata.file_name = ori_file_name;
				metadata.hash_key = GET_INDEX_IN_BOUNDARY(which_server);
				metadata.seq = block_seq;
				metadata.size = block_size;

				metadata.replica = replica;
				metadata.node = nodes[which_server];
				metadata.l_node = nodes[(which_server-1+NUM_NODES)%NUM_NODES];
				metadata.r_node = nodes[(which_server+1+NUM_NODES)%NUM_NODES];
				metadata.is_committed = 1;

				blocks_metadata.push_back(metadata);

				IOoperation io_ops;
				io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_INSERT;
				io_ops.block.first = metadata.name;
				io_ops.block.second = move(sbuffer);

				auto block_server = connect(GET_INDEX(metadata.hash_key));
				send_message(block_server.get(), &io_ops);
				auto reply = read_reply<Reply> (block_server.get());
				block_server->close();

				if (reply->message != "OK") {
					cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
					return EXIT_FAILURE;
				} 
				to_write_byte -= write_length;
				write_byte_cnt += write_length;
				if (to_write_byte == 0) { 
					break;
				}
				start = end;
				end = start + BLOCK_SIZE - 1;
				which_server = (which_server + 1) % NUM_NODES;
			}
		}
		FileUpdate fu;
		fu.name = ori_file_name;
		fu.num_block = block_seq+1;
		fu.size = fd->size + new_file_size;
		fu.blocks_metadata = blocks_metadata;

		send_message(socket.get(), &fu);
		auto reply = read_reply<Reply> (socket.get());
		socket->close();

		if (reply->message != "OK") {
			cerr << "[ERR] Failed to append file. Details: " << reply->details << endl;
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}
	/// }}} 
	// exists {{{
	bool DFS::exists(std::string name) {
		FileExist fe;
		fe.name = name;
		uint32_t hash_key = h(name);
		auto socket = connect(hash_key);
		send_message(socket.get(), &fe);
		auto rep = read_reply<Reply> (socket.get());
		socket->close();

		return (rep->message == "TRUE");
	}
	// }}}
	// touch {{{
	bool DFS::touch(std::string file_name) {
		if (exists(file_name))
			return false;

		uint32_t file_hash_key = h(file_name);

		//! Insert the file
		FileInfo file_info;
		file_info.name = file_name;
		file_info.hash_key = file_hash_key;
		file_info.replica = replica;
		file_info.size = 0ul;

		//! Send file to be submitted;
		auto socket = connect(file_hash_key);
		send_message(socket.get(), &file_info);

		//! Get information of where to send the file
		auto description = read_reply<FileDescription>(socket.get());
		socket->close();

		IOoperation io_ops;
		io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_INSERT;

		int which_server = (description->hash_key % NUM_NODES);
		BlockMetadata metadata;
		metadata.name = file_name + "_H" + to_string(which_server);
		metadata.file_name = file_name;
		metadata.hash_key = GET_INDEX_IN_BOUNDARY(which_server);
		metadata.seq = 0;
		metadata.size = 0;
		metadata.replica = replica;
		metadata.node = nodes[which_server];
		metadata.l_node = nodes[(which_server - 1 + NUM_NODES) % NUM_NODES];
		metadata.r_node = nodes[(which_server + 1 + NUM_NODES) % NUM_NODES];
		metadata.is_committed = 1;

		ChunkMetadata Chunk;
		Chunk.name = file_name + "_" + to_string(0);
		Chunk.primary_file = metadata.name;
		Chunk.size = 0;
		Chunk.chunk_seq = 0;
		Chunk.primary_seq = 0;
		Chunk.offset = 0;
		Chunk.foffset = 0;

		metadata.chunks.push_back(Chunk);

		Block block;
		block.first = metadata.name;
		block.second = "";

		io_ops.block = move(block);

		socket = connect(GET_INDEX(metadata.hash_key));
		send_message(socket.get(), &io_ops);

		auto future = async(launch::async, [](unique_ptr<tcp::socket> socket) -> bool {
				auto reply = read_reply<Reply> (socket.get());
				socket->close();

				if (reply->message != "TRUE") {
				cerr << "[ERR] Failed to upload block . Details: " << reply->details << endl;
				return false;
				}

				return true;
				}, move(socket));

		future.get();

		file_info.num_block = 1;
		file_info.num_primary_file = 1;
		file_info.blocks_metadata.push_back(metadata);
		file_info.uploading = 0;

		socket = connect(file_hash_key);
		send_message(socket.get(), &file_info);
		auto reply = read_reply<Reply> (socket.get());

		if (reply->message != "TRUE") {
			cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
			return EXIT_FAILURE;
		}

		socket->close();

		return EXIT_SUCCESS;
	}

	// }}}

	/* Binary Search Tree for finding Small Block Seq 
	   int retrieve_chunk(uint64_t offset, vector<uint64_t> file_offset, vector<uint32_t> block_size ){
	   int vector_size = (int)file_offset.size() - 1;

	   int start = 0, end = vector_size;
	   int cur = (start + end) / 2;
	   bool exist = false;
	   while(true){
	   if( offset >= file_offset[cur] && offset < file_offset[cur] + (uint64_t)block_size[cur]){ // find location
	   exist = true;
	   break;
	   }

	   if( offset < file_offset[cur] )	end = cur;
	   else start = cur;

	   cur = (start + end) / 2;
	   }

	   return exist ? cur : -1 ;
	   }
	 */

	uint64_t parsing_input(const char* buf, uint64_t to_write_bytes, uint64_t written_bytes, bool is_equal_sized){

		uint64_t parse_idx = to_write_bytes + written_bytes - 1;
		if(parse_idx == -1)
			return -1;

		uint64_t sub_len = 0;
		if (is_equal_sized == false) {
			while (buf[parse_idx] != '\n') {
				sub_len++;
				parse_idx--;
				if(parse_idx == written_bytes){
					break;
				}
			}
		}
		return sub_len; 
	}

	// write {{{
	uint64_t DFS::write(std::string& file_name, const char* buf, uint64_t off, uint64_t len) {
		uint64_t BLOCK_SIZE = context.settings.get<int>("filesystem.block");
		return write(file_name, buf, off, len, BLOCK_SIZE);
	}

	/* Assume : len <= block_size */
	uint64_t DFS::write(std::string& file_name, const char* buf, uint64_t off, uint64_t len, uint64_t block_size) {
		INFO("Start writing %s len %ld off %ld blocksize %ld", file_name.c_str(), len, off, block_size);
		auto socket = connect(h(file_name));

		/* Get FildDescription */
		FileRequest fr;
		fr.name = file_name;
		send_message(socket.get(), &fr);
		auto fd = (read_reply<FileDescription> (socket.get()));
		socket->close();

		if(fd == nullptr) return 0;

		/* Init Data for File Update */

		vector<future<bool>> slave_sockets;

		uint64_t to_write_bytes = len;	// Remaining bytes for writing
		uint64_t written_bytes = 0ul;	  // Written bytes until now
		uint64_t pos_to_update = off, file_pos_to_update = off; // Position(Offset) to be updated in primary & file

		uint64_t len_to_write; // Bytes to be written in sblock

		/* Find beg_seq and end_seq by using Binary Search Tree */
		auto& file_off = fd->offsets_in_file;
		auto& sblock_size = fd->block_size;

		uint32_t sblock_num = fd->num_block; 
		uint32_t sblock_seq = sblock_num - 1; 
		uint32_t primary_num = fd->num_primary_file;
		uint32_t updated_primary_num = 0, updated_sblock_num = 0;

		int which_server;
		BlockMetadata primary_metadata[NUM_NODES];
		bool updated_meta[NUM_NODES] = {0};

		long parsing_size = 0;
		while((long)to_write_bytes > 0){
			ChunkMetadata sblock_metadata;
			Block chunk;

			if(sblock_seq == sblock_num - 1 ){  // Update Sblock
				len_to_write = std::min(block_size - fd->block_size[sblock_seq] - written_bytes, to_write_bytes);
				//parsing_size = parsing_input(buf, len_to_write, fd->block_size[sblock_seq] % block_size, false);
				parsing_size = parsing_input(buf, len_to_write, written_bytes, false);
				len_to_write -= (uint64_t)parsing_size;

				which_server = GET_INDEX(fd->hash_keys[sblock_seq]);
				if(updated_meta[which_server] || parsing_size == -1){
					sblock_seq++;
					continue;
				}

				pos_to_update = fd->offsets[sblock_seq] + fd->block_size[sblock_seq] + written_bytes;

				//! Update Huge Block
				if(!updated_meta[which_server]){
					primary_metadata[which_server].name = fd->primary_files[sblock_seq];
					primary_metadata[which_server].file_name = file_name;
					primary_metadata[which_server].hash_key = fd->hash_keys[sblock_seq];
					primary_metadata[which_server].seq = fd->primary_sequences[sblock_seq];
					primary_metadata[which_server].replica = fd->replica;
					primary_metadata[which_server].node = nodes[which_server];
					primary_metadata[which_server].l_node = nodes[(which_server - 1 + NUM_NODES) % NUM_NODES];
					primary_metadata[which_server].r_node = nodes[(which_server + 1 + NUM_NODES) % NUM_NODES];
					primary_metadata[which_server].is_committed = 1;

					updated_meta[which_server] = true;
				}

				sblock_metadata.offset = fd->offsets[sblock_seq];
				sblock_metadata.foffset = fd->offsets_in_file[sblock_seq];
				sblock_metadata.size = fd->block_size[sblock_seq] + len_to_write;
				sblock_metadata.chunk_seq = fd->chunk_sequences[sblock_seq];
				sblock_metadata.primary_seq = fd->primary_sequences[sblock_seq];
				sblock_metadata.name = fd->blocks[sblock_seq];
				sblock_metadata.primary_file = fd->primary_files[sblock_seq];

				primary_metadata[which_server].size = fd->offsets_in_file[sblock_seq] + fd->block_size[sblock_seq] + len_to_write;
				primary_metadata[which_server].chunks.push_back(sblock_metadata);
			} else { // Insert New Sblock
				len_to_write = (to_write_bytes < block_size) ? to_write_bytes : block_size - parsing_input(buf, block_size, written_bytes, false);

				//! Update Huge Block
				uint32_t primary_seq;
				if(sblock_num >= NUM_NODES) { // If Primary for curruent sblock exists, just update primary metadata.
					primary_seq = (sblock_num - primary_num) % NUM_NODES;
					uint32_t prev_sblock_seq = sblock_num - primary_num;
					which_server = GET_INDEX(fd->hash_keys[primary_seq]);
					pos_to_update = fd->offsets[prev_sblock_seq] + fd->block_size[prev_sblock_seq];
					if(!updated_meta[which_server]){

						primary_metadata[which_server].name = fd->primary_files[prev_sblock_seq];
						primary_metadata[which_server].file_name = file_name;
						primary_metadata[which_server].hash_key = fd->hash_keys[prev_sblock_seq];
						primary_metadata[which_server].seq = fd->primary_sequences[prev_sblock_seq];
						primary_metadata[which_server].replica = fd->replica;
						primary_metadata[which_server].node = nodes[which_server];
						primary_metadata[which_server].l_node = nodes[(which_server - 1 + NUM_NODES) % NUM_NODES];
						primary_metadata[which_server].r_node = nodes[(which_server + 1 + NUM_NODES) % NUM_NODES];
						primary_metadata[which_server].is_committed = 1;

						updated_meta[which_server] = true;
					}

					sblock_metadata.offset = fd->offsets[prev_sblock_seq] + fd->block_size[prev_sblock_seq];
					sblock_metadata.foffset = fd->offsets_in_file[sblock_seq-1] + fd->block_size[sblock_seq-1];
					sblock_metadata.size = len_to_write;
					sblock_metadata.chunk_seq = sblock_seq;
					sblock_metadata.primary_seq = fd->primary_sequences[prev_sblock_seq];
					sblock_metadata.name = file_name + string("_") + to_string(sblock_seq);
					sblock_metadata.primary_file = fd->primary_files[prev_sblock_seq];

					primary_metadata[which_server].size = fd->offsets_in_file[prev_sblock_seq] + fd->block_size[prev_sblock_seq] + len_to_write;
					primary_metadata[which_server].chunks.push_back(sblock_metadata);
				} else { // If not, make a new primary.
					primary_seq = primary_num + updated_primary_num;
					//	which_server = (GET_INDEX(fd->hash_keys[sblock_num -1]) + updated_sblock_num) % NUM_NODES;
					which_server = (which_server + 1) % NUM_NODES;
					pos_to_update = 0;	

					if(!updated_meta[which_server]){
						//primary_metadata[which_server].name = file_name + string("_H") + to_string(which_server);
						primary_metadata[which_server].name = string("Primary_") + to_string(which_server) + string("_") + file_name;
						primary_metadata[which_server].file_name = file_name;
						primary_metadata[which_server].hash_key = GET_INDEX_IN_BOUNDARY(which_server);
						primary_metadata[which_server].seq = primary_seq;
						primary_metadata[which_server].replica = fd->replica;
						primary_metadata[which_server].node = nodes[which_server];
						primary_metadata[which_server].l_node = nodes[(which_server - 1 + NUM_NODES) % NUM_NODES];
						primary_metadata[which_server].r_node = nodes[(which_server + 1 + NUM_NODES) % NUM_NODES];
						primary_metadata[which_server].is_committed = 1;

						updated_meta[which_server] = true;
						updated_primary_num++;

					}

					sblock_metadata.offset = 0;
					sblock_metadata.foffset = fd->offsets_in_file[sblock_seq-1] + fd->block_size[sblock_seq-1];
					sblock_metadata.size = len_to_write;
					sblock_metadata.chunk_seq = sblock_seq;
					sblock_metadata.primary_seq = primary_seq++;
					sblock_metadata.name = file_name + string("_") + to_string(sblock_seq);
					sblock_metadata.primary_file = primary_metadata[which_server].name;

					primary_metadata[which_server].size = len_to_write;
					primary_metadata[which_server].chunks.push_back(sblock_metadata);
				}
				updated_sblock_num++;
				sblock_seq++;
			}

			cout << " Node : " << which_server << " ,SSEQ : " << sblock_metadata.chunk_seq <<" ,HSEQ : " << sblock_metadata.primary_seq <<" ,BOff : " << sblock_metadata.offset <<  " ,Len : " << len_to_write << endl;

			chunk.first = primary_metadata[which_server].name;
			chunk.second = string(buf + written_bytes, len_to_write);

			IOoperation io_ops;
			io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_UPDATE;
			io_ops.pos = pos_to_update;
			io_ops.length = len_to_write;
			io_ops.block = move(chunk);
			socket = connect(GET_INDEX(primary_metadata[which_server].hash_key));
			send_message(socket.get(), &io_ops);

			auto future = async(launch::async, [](unique_ptr<tcp::socket> socket) -> bool {
					auto reply = read_reply<Reply> (socket.get());
					socket->close();

					if (reply->message != "TRUE") {
					cerr << "[ERR] Failed to upload block . Details: " << reply->details << endl;
					return false;
					}

					return true;
					}, move(socket));

			slave_sockets.push_back(move(future));

			written_bytes += len_to_write;
			to_write_bytes -= len_to_write;
		}

		vector<BlockMetadata> update_block_metadata;

		uint32_t min_sblock_seq = -1, beg_insert = -1; // Max_Unsigned_Int
		for(int i = 0; i<NUM_NODES; i++){
			if(updated_meta[i]){
				if(primary_metadata[i].chunks.back().chunk_seq < min_sblock_seq){
					min_sblock_seq = primary_metadata[i].chunks.back().chunk_seq;
					beg_insert = i;
				}
			}
		}

		for(int i = beg_insert; i< NUM_NODES + beg_insert ;  i++){
			uint32_t idx = i % NUM_NODES;
			if(updated_meta[idx]){
				update_block_metadata.push_back(primary_metadata[idx]);
			}
		}

		for (auto& future: slave_sockets)
			future.get();

		// Update Metadata
		FileUpdate fu;
		fu.name = fd->name;
		fu.num_block = fd->num_block + updated_sblock_num;
		fu.num_primary_file = fd->num_primary_file + updated_primary_num;
		fu.size = std::max(written_bytes + off, fd->size);
		fu.blocks_metadata = update_block_metadata;

		socket = connect(fd->hash_key);
		send_message(socket.get(), &fu);
		auto reply = read_reply<Reply> (socket.get());
		socket->close();

		auto iter = file_description_cache.find(file_name);
		if(iter != file_description_cache.end())
			file_description_cache.erase(iter);
		return written_bytes;
	}
	// }}}
	// read {{{
	uint64_t DFS::read(std::string& file_name, char* buf, uint64_t off, uint64_t len) {
		auto fd = get_file_description( std::bind(&connect, std::placeholders::_1), file_name, true);

		if (fd == nullptr) {
			return 0;
		}

		// If it is an MapReduce input file
		if (fd->is_input) {
			return read_logical(file_name, buf, off, len, fd.get());

			// If it is just a regular file
		} else {
			return read_physical(file_name, buf, off, len, fd.get());
		}
	}
	// }}}
	// make_metadata {{{
	model::metadata make_metadata(FileInfo* fi) {
		//	cout<<"******Start Make_Metadata"<<endl;
		model::metadata md;
		if(FileDescription* fd = dynamic_cast<FileDescription*>(fi)) {
			//	cout<<"******FD is exisited "<< fd->name<<endl;
			md.name = fd->name;
			md.hash_key = fd->hash_key;
			md.size = fd->size;
			md.num_block = fd->n_lblock;
			md.type = fd->type;
			md.replica = fd->replica;

			// TODO: They must be removed
			md.blocks = fd->blocks;
			md.hash_keys = fd->hash_keys;
			md.block_size = fd->block_size;

			// set block metadata
			//cout<<"####### NumBlocks: "<< fd->num_block<<endl;
			for(int i=0; i<(int)fd->num_block; i++) {
				model::block_metadata bdata;
				bdata.name = fd->blocks[i];//fd->block_hosts[i] + ":" + to_string(port);
				bdata.size = fd->block_size[i];
				bdata.host = fd->block_hosts[i];
				bdata.index = i;
				bdata.file_name = fd->name;

				bdata.primary_file = fd->primary_files[i];
				bdata.offset = fd->offsets[i];
				bdata.foffset = fd->offsets_in_file[i];
				bdata.primary_seq = fd->primary_sequences[i];

				md.block_data.push_back(std::move(bdata));
			}
		}
		else {
			md.name = fi->name;
			md.hash_key = fi->hash_key;
			md.size = fi->size;
			md.num_block = fi->num_block;
			md.type = fi->type;
			md.replica = fi->replica;
			md.has_block_data = false;
		}

		return std::move(md);
	}
	// }}}
	// get_metadata {{{
	model::metadata DFS::get_metadata(std::string& fname) {
		auto fd = get_file_description( std::bind(&connect, std::placeholders::_1), fname);
		if(fd != nullptr) {
			FileInfo& fi = *fd;

			return make_metadata(&fi);
		}
		else
			return model::metadata();
	}
	// }}}
	// get_metadata_optimized {{{
	model::metadata DFS::get_metadata_optimized(std::string& fname, int type) {
		bool is_logical_blocks = GET_STR("addons.zk.enabled") == string("true");
		if (!is_logical_blocks or type == VELOX_LOGICAL_DISABLE) {
			return get_metadata(fname);
		}
		// Generate or read logical blocks metadata from Fileleader
		FileRequest fr;
		fr.name = fname;
		fr.type = "LOGICAL_BLOCKS";
		fr.generate = (type == VELOX_LOGICAL_GENERATE);

		auto socket = connect(h(fname));
		send_message(socket.get(), &fr);
		auto fd = (read_reply<FileDescription> (socket.get()));

		socket->close();

		if (fd == nullptr) {
			cerr << "FileLeader returned empty FileDescription, exiting..." << endl;
			exit(EXIT_SUCCESS);
		}

		// If the file is not input file
		if (fd->is_input == false) {
			return get_metadata(fname);
		}

		// Fill up model::metadata
		model::metadata md;
		md.name = fd->name;
		md.hash_key = fd->hash_key;
		md.size = fd->size;

		md.num_static_blocks = fd->num_static_blocks;
		md.num_chunks = fd->num_block; // Total number of Chunks
		md.num_block = fd->n_lblock;    // Total Number of logical blocks
		md.type = fd->type;
		md.replica = fd->replica;

		if (type == VELOX_LOGICAL_NOOP) {
			md.num_block = 0;
			return md;
		}
		
		for (auto& lblock : fd->logical_blocks) {
			model::block_metadata bdata;
			bdata.name      = lblock.name;
			bdata.size      = lblock.size;
			bdata.host      = lblock.host_name;
			bdata.index     = lblock.seq;
			bdata.primary_chunk_num = lblock.primary_chunk_num;
			bdata.file_name = lblock.file_name;
			
			md.block_data.push_back(bdata);
		}
		
		for(auto& lblock : fd->logical_blocks){
			TaskOperation task_ops;
			task_ops.operation = eclipse::messages::TaskOperation::OpType::TASK_INIT;
			task_ops.file = lblock.physical_blocks[0].primary_file;
			task_ops.job_id = mr_job_id;
			task_ops.tmg_id = tmg_id;
			task_ops.lblock_metadata = lblock;
			
			auto slave_socket = connect_host(lblock.host_name);
			send_message(slave_socket.get(), &task_ops);
			slave_socket->close();
		} 

		return md;
	}
	// }}}
	// get_metadata_all {{{
	vector<model::metadata> DFS::get_metadata_all() {
		vector<FileInfo> total; 

		for (unsigned int net_id=0; net_id<NUM_NODES; net_id++) {
			FileList file_list;
			auto socket = connect(net_id);
			send_message(socket.get(), &file_list);
			auto file_list_reply = read_reply<FileList>(socket.get());
			std::copy(file_list_reply->data.begin(), file_list_reply->data.end(), back_inserter(total));
		}

		vector<model::metadata> metadata_vector;

		for (auto fd : total) {
			metadata_vector.push_back(make_metadata(&fd));
		}

		return move(metadata_vector);
	}
	// }}}
	// file_metadata_append {{{
	void DFS::file_metadata_append(std::string name, size_t size, model::metadata& blocks) {
		FileUpdate fu;
		fu.name = name;
		fu.num_block = blocks.blocks.size();
		fu.size = size;
		fu.is_append = true;

		for (size_t i = 0; i < blocks.blocks.size(); i++) {
			BlockMetadata metadata;
			metadata.file_name = name;
			metadata.name = blocks.blocks[i];
			metadata.seq = 0;
			metadata.hash_key = blocks.hash_keys[i];
			metadata.size = blocks.block_size[i];
			metadata.replica = 1;
			metadata.type = 0;
			metadata.node = "";
			metadata.l_node = "";
			metadata.r_node = "";
			metadata.is_committed = 1;

			fu.blocks_metadata.push_back(metadata);
		}


		uint32_t file_hash_key = h(name);
		auto socket = connect(file_hash_key);
		send_message(socket.get(), &fu);
		read_reply<Reply>(socket.get());
		socket->close();
	}
	// }}}
	// dump_metadata {{{
	std::string DFS::dump_metadata(std::string& fname) {
		const uint32_t s4KB = 4 << 10;
		char output [s4KB];

		auto fd = get_file_description( std::bind(&connect, std::placeholders::_1), fname);

		if (fd != nullptr) {
			snprintf(output, s4KB,
					R"(
  name = %s;
  hash_key = %u;
  size = %lu;
  num_block = %u;
  n_lblock = %u;
  type = %u;
  replica = %u;
  reducer_output = %u;
  job_id = %u;
  uploading = %u;
  is_input = %u;
  intended_block_size = %lu;
)",
					fd->name.c_str(),
					fd->hash_key,
					fd->size,
					fd->num_block,
					fd->n_lblock,
					fd->type,
					fd->replica,
					fd->reducer_output,
					fd->job_id,
					fd->uploading,
					fd->is_input,
					fd->intended_block_size
						);
		} else {
			cout << "I could not find the file: " << fname << endl;
		}

		return string(output);
	}
	
	// }}}
	// read_chunk {{{
	uint64_t DFS::read_chunk(char* buf, uint32_t buffer_offset) {
		uint64_t read_bytes = 0;
		struct shm_buf* cur_index;
		int processed_buf = 0;
		int begin_check_idx = shm_buf_idx;	
			
		pthread_mutex_lock(&sema->lock);
		while(sema->tail - sema->head == 0){
			if(*(bool*)shm_status_addr){
				pthread_mutex_unlock(&sema->lock);
				return 0;
			}
			pthread_cond_wait(&sema->nonzero, &sema->lock);
		}

		pthread_cond_signal(&sema->nonzero);

		cur_index = (struct shm_buf*)(shm_idx_addr + sizeof(struct shm_buf) * (sema->head % sema->queue_size) );
		char* buf_addr = (char*)(shm_base_chunk_addr + BLOCK_SIZE * (sema->head % sema->queue_size) );
		strncpy(buf, &(buf_addr[0]), cur_index->chunk_size);
		read_bytes = cur_index->chunk_size;
		memset(&(buf_addr[0]), 0, cur_index->chunk_size);
		
		sema->head++;
		pthread_mutex_unlock(&sema->lock);

		processed_chunk++;
		if(to_del_buf != NULL) delete[] to_del_buf;
		to_del_buf = buf;
		
		return read_bytes;
	}
	// }}}


	uint64_t DFS::write_chunk(std::string& file_name, const char* buf, uint64_t off, uint64_t len, uint64_t block_size, uint32_t sblock_seq) {
		INFO("Start writing %s len %ld off %ld blocksize %ld", file_name.c_str(), len, off, block_size);
		bool is_equal_sized_blocks = (GET_STR("filesystem.equal_sized_blocks") == "true");
		auto socket = connect(h(file_name));

		FileRequest fr;
		fr.name = file_name;
		send_message(socket.get(), &fr);
		auto fd = (read_reply<FileDescription> (socket.get()));
		socket->close();

		if(fd == nullptr) return 0;

		//! Insert the blocks
		BlockMetadata blocks_metadata;
		vector<future<bool>> slave_sockets;

		uint64_t to_write_bytes = len;
		uint64_t written_bytes = 0ul;

		int primary_seq = 0; 
		int primary_num = (int)fd->num_primary_file, num_updated_primary_file = (int)0;
		int sblock_num = (int)fd->num_block;
		int last_chunk_seq = -1;	

		for(int i = sblock_num -1; i >= 0 && primary_num > 0; --i){
			int h_idx = GET_INDEX(fd->hash_keys[i]);
			last_chunk_seq = i;
			primary_num--;
			num_updated_primary_file++;
		}

		uint64_t pos_to_update, len_to_write, file_pos_to_update = off;

		int which_server; 
		ChunkMetadata metadata;
		Block chunk;
		IOoperation io_ops;
		io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_UPDATE;

		if( (int)fd->num_block  == 1)  { // For Block Update Request
			cout << "Update >>" ;
			which_server = GET_INDEX(fd->hash_keys[0]);// % NUM_NODES;
			blocks_metadata.name = fd->primary_files[0];
			blocks_metadata.file_name = file_name;
			blocks_metadata.hash_key = fd->hash_keys[0];
			blocks_metadata.seq = fd->primary_sequences[0]; // it is not important
			blocks_metadata.replica = fd->replica;
			blocks_metadata.node = nodes[which_server];
			blocks_metadata.l_node = nodes[(which_server-1+NUM_NODES)%NUM_NODES];
			blocks_metadata.r_node = nodes[(which_server+1+NUM_NODES)%NUM_NODES];
			blocks_metadata.is_committed = 1;

			primary_seq++;
			INFO("Update new block size %i", len_to_write);
			pos_to_update = file_pos_to_update - fd->offsets_in_file[0] + fd->offsets[0]; 
			len_to_write = (fd->block_size[0] == 0) ? std::min(to_write_bytes, block_size) : std::min((block_size - pos_to_update), to_write_bytes);
			cout << "write len : "  << len_to_write << endl;
			//len_to_write -= parsing_input(buf, len_to_write, is_equal_sized_blocks);
			metadata.offset = fd->offsets[0];
			metadata.foffset = fd->offsets_in_file[0];
			metadata.size = std::max(fd->block_size[0], pos_to_update + len_to_write);
			metadata.chunk_seq = fd->chunk_sequences[0];
			metadata.primary_seq = fd->primary_sequences[0];
			metadata.name= fd->blocks[0];
			metadata.primary_file= fd->primary_files[0];

			blocks_metadata.size += max(fd->block_size[0], pos_to_update + len_to_write) - fd->block_size[0];
			blocks_metadata.chunks.push_back(metadata);

		} else { // For Block Insert Request
			cout << " Insert >> " ;
			which_server = (which_server + 1) % NUM_NODES;
			blocks_metadata.name = file_name + "_H" + to_string(which_server);
			blocks_metadata.file_name = file_name;
			blocks_metadata.hash_key = GET_INDEX_IN_BOUNDARY(which_server);
			blocks_metadata.seq = primary_seq++; 
			blocks_metadata.replica = fd->replica;
			blocks_metadata.node = nodes[which_server];
			blocks_metadata.l_node = nodes[(which_server-1+NUM_NODES)%NUM_NODES];
			blocks_metadata.r_node = nodes[(which_server+1+NUM_NODES)%NUM_NODES];
			blocks_metadata.is_committed = 1;

			INFO("Insert new block size %i", len_to_write);

			int Idx = last_chunk_seq;
			pos_to_update = Idx < 0 ? 0 : fd->offsets[Idx] + fd->block_size[Idx];
			len_to_write = std::min(block_size, to_write_bytes); 
			//len_to_write -= parsing_input(buf, len_to_write, is_equal_sized_blocks);
			cout << "write len : "  << len_to_write << endl;
			blocks_metadata.size += len_to_write; //std::max(fd->block_size[i] , pos_to_update + len_to_write);
			metadata.offset = pos_to_update; 
			metadata.foffset = file_pos_to_update;
			metadata.size = len_to_write;
			metadata.chunk_seq = sblock_seq;
			metadata.primary_seq = blocks_metadata.seq ;
			metadata.name = file_name + string("_") + to_string(sblock_seq);
			metadata.primary_file= blocks_metadata.name;

			blocks_metadata.chunks.push_back(metadata);

		}

		chunk.first = blocks_metadata.name;
		chunk.second = string(buf + written_bytes, len_to_write);
		io_ops.pos = pos_to_update;
		io_ops.length = len_to_write;
		io_ops.block = move(chunk);

		cout << " which_server : " << which_server << " Seq: " << metadata.chunk_seq <<" offset : " <<pos_to_update<<" File offset : "<< file_pos_to_update << "to_write_bytes : " << to_write_bytes << endl;
		socket = connect(GET_INDEX(blocks_metadata.hash_key));
		send_message(socket.get(), &io_ops);

		auto future = async(launch::async, [](unique_ptr<tcp::socket> socket) -> bool {
				auto reply = read_reply<Reply> (socket.get());
				socket->close();

				if (reply->message != "TRUE") {
				cerr << "[ERR] Failed to upload block . Details: " << reply->details << endl;
				return false;
				}

				return true;
				}, move(socket));

		slave_sockets.push_back(move(future));

		file_pos_to_update += len_to_write;	
		written_bytes += len_to_write;
		to_write_bytes -= len_to_write;
		cout << "Written Bytes: " <<written_bytes << " Remained : " << (long)to_write_bytes << endl;


		for (auto& future: slave_sockets)
			future.get();

		// Update Metadata
		FileUpdate fu;
		fu.name = fd->name;
		fu.num_block = fd->num_block + 1;
		fu.num_primary_file = std::max((unsigned int)fd->num_primary_file, (unsigned int)num_updated_primary_file);
		fu.size = std::max(written_bytes + off, fd->size);
		// fu.blocks_metadata = blocks_metadata;
		fu.blocks_metadata.push_back(blocks_metadata);

		socket = connect(fd->hash_key);
		send_message(socket.get(), &fu);
		auto reply = read_reply<Reply> (socket.get());
		socket->close();

		auto iter = file_description_cache.find(file_name);
		if(iter != file_description_cache.end())
			file_description_cache.erase(iter);

		return written_bytes;

	}	

	int DFS::write_file(std::string file_name, bool is_binary, const std::string& buf, uint64_t len) {
		cout << " In Place Upload" << endl;
		uint64_t BLOCK_SIZE = context.settings.get<int>("filesystem.block");
		int replica = GET_INT("filesystem.replica");
		bool is_equal_sized_blocks = (GET_STR("filesystem.equal_sized_blocks") == "true");

		if (is_binary) {
			replica = NUM_NODES;
		}

		//! Does the file exists
		if (this->exists(file_name)) {
			cerr << "[ERR] " << file_name << " already exists in VeloxDFS." << endl;
			return EXIT_FAILURE;
		}

		//! Insert the file

		uint32_t file_hash_key = h(file_name);	
		FileInfo file_info;
		file_info.name = file_name;
		file_info.hash_key = file_hash_key;
		file_info.replica = replica;
		file_info.size = len;
		file_info.is_input = true;
		file_info.uploading = 1;
		file_info.intended_block_size = BLOCK_SIZE;

		//! Send file to be submitted;
		auto socket = connect(file_hash_key);
		send_message(socket.get(), &file_info);

		//! Get information of where to send the file
		auto description = read_reply<FileDescription>(socket.get());
		socket->close();

		uint64_t start = 0;
		uint64_t end = start + BLOCK_SIZE;
		uint64_t current_block_size = 0;
		unsigned int block_seq = 0;

		//! Insert the blocks
		uint32_t i = 0;

		//vector<BlockMetadata> blocks_metadata;
		Block blocks[NUM_NODES]; 
		std::map<int, BlockMetadata> blocks_metadata;	
		std::map<int, BlockMetadata>::iterator iter;
		vector<future<bool>> slave_sockets;
		vector<char> chunk(BLOCK_SIZE);
		unsigned int Primary_seq = 0;

		cout << " Insert the Blocks" << endl;
		while (true) {
			if (end < file_info.size) {
				if (is_equal_sized_blocks == false) {
					while (buf[--end] != '\n') {
					}
				}
			} else {
				end = len;
			}

			BlockMetadata metadata;
			Block block;
			ChunkMetadata Chunk;
			IOoperation io_ops;

			bool is_first_block = bool((i == 0) && !is_equal_sized_blocks);
			//current_block_size = (uint32_t) end - start + bool(is_first_block);
			current_block_size = end - start + bool(is_first_block);
			block.second.reserve(current_block_size);

			if (is_first_block) {
				block.second[0] = '\n';
			}

			//myfile.read(chunk.data() + is_first_block, current_block_size - bool(is_first_block));
			block.second = move(string(&buf[start + is_first_block], current_block_size - bool(is_first_block)));

			//! Load block metadata info
			int which_server = ((file_hash_key % NUM_NODES) + i) % NUM_NODES;
			iter = blocks_metadata.find(which_server);
			if(iter == blocks_metadata.end()){ //first block 
				blocks_metadata.insert({which_server, metadata});

				//block.first = blocks_metadata[which_server].name = file_name + string("_H") + to_string(which_server);
				block.first = blocks_metadata[which_server].name = string("Primary_") + to_string(which_server) + string("_") + file_name;
				blocks_metadata[which_server].file_name = file_name;
				blocks_metadata[which_server].hash_key = GET_INDEX_IN_BOUNDARY(which_server);
				blocks_metadata[which_server].seq = Primary_seq++;
				blocks_metadata[which_server].size = current_block_size;
				blocks_metadata[which_server].replica = replica;
				blocks_metadata[which_server].node = nodes[which_server];
				blocks_metadata[which_server].l_node = nodes[(which_server-1+NUM_NODES)%NUM_NODES];
				blocks_metadata[which_server].r_node = nodes[(which_server+1+NUM_NODES)%NUM_NODES];
				blocks_metadata[which_server].is_committed = 1;

				Chunk.offset = 0; 
				Chunk.foffset = start; 
				Chunk.size = current_block_size;
				Chunk.chunk_seq = block_seq;
				Chunk.primary_seq = blocks_metadata[which_server].seq;
				Chunk.name = file_name + string("_") + to_string(block_seq);
				Chunk.primary_file = blocks_metadata[which_server].name;

				blocks_metadata[which_server].chunks.push_back(Chunk);

				io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_INSERT;
				io_ops.block = move(block);
				iter = blocks_metadata.find(which_server);
			} else{
				Chunk.offset = iter->second.size;
				Chunk.foffset = start;
				Chunk.size = current_block_size;
				Chunk.chunk_seq = block_seq;
				Chunk.primary_seq = iter->second.seq;
				Chunk.name = file_name + string("_") + to_string(block_seq);
				Chunk.primary_file = iter->second.name;

				block.first = iter->second.name;
				iter->second.chunks.push_back(Chunk);
				iter->second.size += current_block_size;

				io_ops.pos = Chunk.offset;
				io_ops.length = current_block_size;
				io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_UPDATE;
				io_ops.block = move(block);
			}

			auto socket = connect(GET_INDEX(iter->second.hash_key));
			send_message(socket.get(), &io_ops);

			auto future = async(launch::async, [](unique_ptr<tcp::socket> socket) -> bool {

					auto reply = read_reply<Reply> (socket.get());
					socket->close();

					if (reply->message != "TRUE") {
					cerr << "[ERR] Failed to upload block . Details: " << reply->details << endl;
					return false;
					}

					return true;
					}, move(socket));

			slave_sockets.push_back(move(future));

			cout << "IT2 " << block_seq << "START: " << start << " END: " << end << " bs: " << current_block_size << " bl: " <<io_ops.block.second.length() << " VS "<< end - start <<endl;
			if (end >= file_info.size) {
				break;
			}
			start = end;
			end = start + BLOCK_SIZE;
			block_seq++;
			i++;

		}

		std::vector<BlockMetadata> blocksMetadata;
		for(iter = blocks_metadata.begin() ; iter != blocks_metadata.end(); iter++){
			blocksMetadata.push_back(iter->second);
		}

		for (auto& future: slave_sockets)
			future.get();

		file_info.num_primary_file = Primary_seq;
		file_info.num_block = block_seq;
		file_info.blocks_metadata = blocksMetadata;
		file_info.uploading = 0;

		socket = connect(file_hash_key);
		send_message(socket.get(), &file_info);
		auto reply = read_reply<Reply> (socket.get());

		if (reply->message != "TRUE") {
			cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
			return EXIT_FAILURE;
		}

		socket->close();
		//close(fd);

		cout << "[INFO] " << file_name << " is uploaded." << endl;
		cout << "[INFO] file size : " << file_info.size << endl;
		return EXIT_SUCCESS;
	}
}
