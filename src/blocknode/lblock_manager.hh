#pragma once
#include "local_io.hh"
#include "../messages/IOoperation.hh"
#include <string>
#include <mutex>
#include <map>

#define BLOCK_SIZE 8388608

const int default_shm_id = 85549;

struct shm_info {
//	char *buf; //[BLOCK_SIZE];
	char buf[BLOCK_SIZE];
	uint64_t chunk_size;
	uint32_t chunk_index;
	bool commit;
};

struct task {
	std::shraed_ptr<std::ifstream> fd;
	std::vector<eclipse::messages::BlockInfo> assigned_chunks;
};
	
class lblock_manager {
private :
	std::vector<struct task> tasks;
	std::thread lbm_master;
	std::vector<std::thread> lbm_workers;
	std::vector<std::shread_ptr<std::mutex> > locks;
	bool processing;
public :

	lblock_manager() : processing(false);

	void lblock_manager_init_task(string file, std::vector<eclipse::messages::BlockInfo>& assigned_chunks, int task_id);
    void lbm_read(int task_id);
	void lblock_manager_stop_task(int task_id);
	bool lblock_manager_destroy_task(int task_id);
	
	~lblock_manager();

	
};


