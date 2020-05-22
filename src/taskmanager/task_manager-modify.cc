#include "task_manager.hh"
#include <zookeeper/zookeeper.h>
#include <iostream>
#include <algorithm>
#include <sys/time.h>

using namespace std;
using namespace eclipse;
using namespace eclipse::messages;
using namespace eclipse::network;

namespace eclipse {
	struct timeval zk_start, zk_end;
	struct timeval io_start, io_end;
	struct timeval total_start, total_end;
	double zk_time = 0.0;
	double io_time = 0.0;
	double total_time = 0.0;


TaskManager::TaskManager (ClientHandler* net) : Node() {
	network = net;
  	network_size = context.settings.get<vec_str>("network.nodes").size();

}

TaskManager::~TaskManager(){}


// Implemented by using zookeeper 
DistLockStatus get_Dlock(zhandle_t * zh, string znode, bool isPrimary){
	gettimeofday(&zk_start, NULL);
	string znode_contents = isPrimary ? "1" : "0"; 
	char buffer[128] = {0};
	int zoo_get_buf_len = 128;
	int rc = zoo_create(zh, znode.c_str(), znode_contents.c_str(), 1, &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL, buffer, 128);

	if(rc){  // Failed to get a dist lock
		if(!zoo_get(zh, znode.c_str(), 0, buffer, &zoo_get_buf_len, NULL)){
			if(!strncmp(buffer, "1", zoo_get_buf_len)){  // conflict with primary node
				gettimeofday(&zk_end, NULL);
				zk_time += (zk_end.tv_sec - zk_start.tv_sec) + ((double)(zk_end.tv_usec - zk_start.tv_usec) / 1000000);
				//cout << "Replica is End by Conflict : " << znode << endl;
				return END_OF_FILE;		
			} else { 										 // conflict with replica nodes
				gettimeofday(&zk_end, NULL);
				zk_time += (zk_end.tv_sec - zk_start.tv_sec) + ((double)(zk_end.tv_usec - zk_start.tv_usec) / 1000000);
				//cout << "Try next replica block : " << znode << endl;
				return isPrimary ? END_OF_FILE : TRY_NEXT_DIST_LOCK;
			}
		}	
	}
	gettimeofday(&zk_end, NULL);
	zk_time += (zk_end.tv_sec - zk_start.tv_sec) + ((double)(zk_end.tv_usec - zk_start.tv_usec) / 1000000);
	//cout << "Steal from replica : " << znode << endl;
	return GET_DIST_LOCK;
}
 
DistLockStatus get_dist_lock(zhandle_t * zh, string target_node, string znode, bool isPrimary, bool* Stealing){
	gettimeofday(&zk_start, NULL);
	char buffer[128] = {0};
	int rc;
	int zoo_get_buf_len = 128;
	Stat zoo_stat;

	if(!(*Stealing)){
		rc = zoo_exists(zh, target_node.c_str(), 0 , &zoo_stat);
		if(rc != ZOK){
			gettimeofday(&zk_end, NULL);
			zk_time += (zk_end.tv_sec - zk_start.tv_sec) + ((double)(zk_end.tv_usec - zk_start.tv_usec) / 1000000);
			//cout << "Get : " << znode << endl;
			return GET_DIST_LOCK;
		} else {
			DistLockStatus lock_rc = get_Dlock(zh, znode, true);
			zoo_set(zh, target_node.c_str(), "Done", strlen("Done"), -1);
			*Stealing = true;
//			cout << "Primary is done by Confilct : " << znode << endl; 
			return lock_rc;
		}
	} else {
		while(true){
			zoo_get(zh, target_node.c_str(), 0, buffer, &zoo_get_buf_len, NULL);
			
			if(!strcmp(buffer, "Done")){
				break;
			} 		
		}

		return get_Dlock(zh, znode, false);
	}

	gettimeofday(&zk_end, NULL);
	zk_time += (zk_end.tv_sec - zk_start.tv_sec) + ((double)(zk_end.tv_usec - zk_start.tv_usec) / 1000000);
	return GET_DIST_LOCK;
}

//bool produce(ifstream& ifs, messages::BlockInfo& md, struct shm_buf** cur_chunk, int idx){
bool produce(ifstream& ifs, semaphore_t* semap, messages::BlockInfo& md, struct shm_buf** shm){

	pthread_mutex_lock(&semap->lock);

	if((semap->tail - semap->head) < semap->queue_size){
		pthread_cond_signal(&semap->nonzero);
	} else {
		pthread_cond_wait(&semap->nonzero, &semap->lock);
	}

	gettimeofday(&io_start, NULL);
	ifs.read(&(shm[semap->tail % semap->queue_size]->buf[0]), (long)md.size);
	gettimeofday(&io_end, NULL);
	io_time += (io_end.tv_sec - io_start.tv_sec) + ((double)(io_end.tv_usec - io_start.tv_usec) / 1000000);
	shm[semap->tail % semap->queue_size]->chunk_size = md.size;
	shm[semap->tail % semap->queue_size]->chunk_index = md.seq;
	
	semap->tail++;
	pthread_mutex_unlock(&semap->lock);
	
}

void task_worker(std::string file, struct logical_block_metadata& lblock_metadata, string _job_id, int _task_id){
	gettimeofday(&total_start, NULL);
	string job_id = _job_id;

	/* For Shared Memory */
	uint64_t BLOCK_SIZE = context.settings.get<int>("filesystem.block");
	int shm_buf_depth = context.settings.get<int>("addons.shm_buf_depth");
	int shm_buf_width = context.settings.get<int>("addons.shm_buf_width");
	uint64_t buf_pool_size = sizeof(bool) + (sizeof(struct shm_buf) + BLOCK_SIZE) * shm_buf_depth * shm_buf_width;
	string disk_path = context.settings.get<string>("path.scratch") + "/";
	
	/* For Zookeepr */	
	string zk_server_addr = GET_STR("addons.zk.addr");
  	int zk_server_port = GET_INT("addons.zk.port");
  	string addr_port = zk_server_addr + ":" + to_string(zk_server_port);	
	
	zoo_set_debug_level((ZooLogLevel)0);
 	zhandle_t* zh = zookeeper_init(addr_port.c_str(), NULL , 40000, 0, 0, 0);
	if(!zh){
		cout << "[ " << job_id << " ] : " << "Zookeeper Connection Error" << endl;
	}
	string zk_prefix = "/chunks/" + job_id + "/";

	/* For FD*/
	ifstream ifs;
	ifs.open(disk_path + file, ios::in | ios::binary);
	ifs.seekg(ios::beg);

	int input_block_num = lblock_metadata.primary_chunk_num;
	string my = lblock_metadata.host_name;
	auto& md = lblock_metadata.physical_blocks;
	int task_id = _task_id;	

	int shmid;
	void* shared_memory; 
	uint64_t shm_status_addr;
	uint64_t shm_base_addr;
	uint64_t shm_chunk_base_addr;
	
	/* Get Shared Memory Pool */
	struct shm_buf** chunk_index = new struct shm_buf*[shm_buf_width * shm_buf_depth]; 
	shmid = shmget((key_t)(DEFAULT_KEY + task_id), buf_pool_size, 0666|IPC_CREAT);

	if(shmid == -1){
		cout << "shmget failed" << endl;
		exit(1);
	}

	shared_memory = shmat(shmid, NULL, 0);
	if(shared_memory == (void*)-1){
		cout << "shmat failed" << endl;
		exit(1);
	}

	/* For User level semaphore */
	semaphore_t *sema;
	string sema_path = "/tmp/semaphore" + to_string(task_id);

	sema = semaphore_create(sema_path.c_str(), (shm_buf_depth * shm_buf_width) );

	memset(shared_memory, 0, buf_pool_size);
	shm_status_addr = (uint64_t)shared_memory;
	shm_base_addr = (uint64_t)shared_memory + sizeof(bool);
	shm_chunk_base_addr = shm_base_addr + sizeof(struct shm_buf) * shm_buf_width * shm_buf_depth;
	int shm_buf_num = shm_buf_width * shm_buf_depth;

	for(int i = 0; i < shm_buf_num; i++){
		chunk_index[i] = (struct shm_buf*)(shm_base_addr + sizeof(struct shm_buf) * i);
		chunk_index[i]->buf = (char*)(shm_chunk_base_addr + BLOCK_SIZE * i);
	}
	
	int md_index = 0;
	uint64_t input_file_offset = 0, read_bytes = 0;
	bool isPrimary = true;
	bool Stealing = false;
	
	int processed_file_cnt = 0, r_idx = 0, replica_num = 3, shm_idx = 0;
	int process_chunk = 0;
 	//string zk_prefix = "/chunks/" + job_id + "/";
	string zk_eof = zk_prefix + my;

	while( processed_file_cnt < replica_num ) {

		while(md_index < input_block_num){
			int index_to_copy;
			uint64_t replica_offset = 0;

			string zk_path = zk_prefix + to_string(md[md_index].seq);

			DistLockStatus rc = get_dist_lock(zh, zk_eof, zk_path, isPrimary, &Stealing);
			if(rc == GET_DIST_LOCK){

				if(!isPrimary) {
					ifs.seekg(md[md_index].offset);
				}	
				
				produce(ifs, sema, md[md_index], chunk_index);

				process_chunk++;

			} else if(rc == END_OF_FILE){
				md_index = input_block_num;
				break;
			} else if(rc == TRY_NEXT_DIST_LOCK){
			} else {
				exit(static_cast<int>(ZOO_CREATE_ERROR));
			}
			md_index++;
		}

		ifs.close();	
		processed_file_cnt++;
		if(md.size() == md_index) break;
		ifs.open(disk_path + md[md_index].primary_file, ios::in | ios::binary | ios::ate);
		char buffer[512] = {0};
		int rc;
		
		if(isPrimary){
			rc = zoo_create(zh, zk_eof.c_str(), "None", strlen("None"), &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL, buffer, 512);
			zoo_create(zh, (zk_prefix + to_string(md[md_index-1].seq)).c_str(), "1", 1, &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL, buffer, 512);
			zoo_set(zh, zk_eof.c_str(), "Done", strlen("Done"), -1);
		}

		Stealing = true;
		isPrimary = false;
		zk_eof = zk_prefix + md[md_index].node;
		rc = zoo_create(zh, zk_eof.c_str(), "None", strlen("None"), &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL, buffer, 512);
		input_block_num += lblock_metadata.replica_chunk_num[r_idx++];
		input_file_offset = 0;
		gettimeofday(&total_end, NULL);
		total_time = (total_end.tv_sec - total_start.tv_sec) + ((double)(total_end.tv_usec - total_start.tv_usec) / 1000000);
		cout << "Sub time: " << total_time << " zk_time: " << zk_time << " io_time: " << io_time << " process chunk : " << process_chunk << endl;
	}
	
	pthread_mutex_lock(&sema->lock);
	*(bool*)shm_status_addr = true;	
	pthread_mutex_cond_broadcast(&semap->nonzero);
	pthread_mutex_unlock(&sema->lock);

	gettimeofday(&total_end, NULL);
	total_time = (total_end.tv_sec - total_start.tv_sec) + ((double)(total_end.tv_usec - total_start.tv_usec) / 1000000);
	cout << "Total time: " << total_time << " zk_time: " << zk_time << " io_time: " << io_time << " process chunk : " << process_chunk << endl;

	sleep(1000);

  	zookeeper_close(zh);
	/* Close Task */
	cout << "Detach all" << endl;
	delete[] chunk_index;
	shmdt(shared_memory);
	shmctl(shmid, IPC_RMID, 0);

	return;
}

void static_worker(std::string file, struct logical_block_metadata& lblock_metadata, string _job_id, int _task_id){
	gettimeofday(&total_start, NULL);
	string job_id = _job_id;
	/* For Shared Memory */
	uint64_t BLOCK_SIZE = context.settings.get<int>("filesystem.block");
	int shm_buf_depth = context.settings.get<int>("addons.shm_buf_depth");
	int shm_buf_width = context.settings.get<int>("addons.shm_buf_width");
	uint64_t buf_pool_size = sizeof(bool) + (sizeof(struct shm_buf) + BLOCK_SIZE) * shm_buf_depth * shm_buf_width;
	string disk_path = context.settings.get<string>("path.scratch") + "/";
	
	/* For FD*/
	ifstream ifs;
	ifs.open(disk_path + file, ios::in | ios::binary);
	ifs.seekg(ios::beg);

	int input_block_num = lblock_metadata.primary_chunk_num;
	auto& md = lblock_metadata.physical_blocks;
	int task_id = _task_id;	

	int shmid;
	void* shared_memory; 
	uint64_t shm_status_addr;
	uint64_t shm_base_addr;
	uint64_t shm_chunk_base_addr;
	
	/* Get Shared Memory Pool */
	struct shm_buf** chunk_index = new struct shm_buf*[shm_buf_width * shm_buf_depth]; 
	shmid = shmget((key_t)(DEFAULT_KEY + task_id), buf_pool_size, 0666|IPC_CREAT);

	if(shmid == -1){
		cout << "shmget failed" << endl;
		exit(1);
	}

	shared_memory = shmat(shmid, NULL, 0);
	if(shared_memory == (void*)-1){
		cout << "shmat failed" << endl;
		exit(1);
	}
	memset(shared_memory, 0, buf_pool_size);
	shm_status_addr = (uint64_t)shared_memory;
	shm_base_addr = (uint64_t)shared_memory + sizeof(bool);
	shm_chunk_base_addr = shm_base_addr + sizeof(struct shm_buf) * shm_buf_width * shm_buf_depth;
	int shm_buf_num = shm_buf_width * shm_buf_depth;

	/* Init Mutex Locks for each shm_buf && store chunk addr*/
	pthread_mutexattr_t lock_attr;
	pthread_mutexattr_init(&lock_attr);
	pthread_mutexattr_setpshared(&lock_attr, PTHREAD_PROCESS_SHARED);
	for(int i = 0; i < shm_buf_num; i++){
		chunk_index[i] = (struct shm_buf*)(shm_base_addr + sizeof(struct shm_buf) * i);
		pthread_mutex_init(&(chunk_index[i]->lock), &lock_attr);
		chunk_index[i]->buf = (char*)(shm_chunk_base_addr + BLOCK_SIZE * i);
	}
	
	int md_index = 0;
	uint64_t input_file_offset = 0, read_bytes = 0;
	bool isPrimary = true;

	int processed_file_cnt = 0, r_idx = 0, replica_num = 1, shm_idx = 0;

	while(md_index < input_block_num){
		produce(ifs, sema, md[md_index], chunk_index);
		md_index++;
	}

	ifs.close();	

	*(bool*)shm_status_addr = true;	
	gettimeofday(&total_end, NULL);
	total_time = (total_end.tv_sec - total_start.tv_sec) + ((double)(total_end.tv_usec - total_start.tv_usec) / 1000000);
	cout << "Total time: " << total_time << " zk_time: " << zk_time << " io_time: " << io_time << endl;

	sleep(1000);

	/* Close Task */
	delete[] chunk_index;
	shmdt(shared_memory);
	shmctl(shmid, IPC_RMID, 0);
		

	return;
}
void TaskManager::task_init(std::string file, struct logical_block_metadata& metadata, string job_id, int _task_id){
	string policy = GET_STR("addons.job_policy");
	cout << "Policy : " << policy << endl;
	if(policy == "static"){
		std::thread worker = thread(&static_worker, file, std::ref(metadata), job_id, _task_id);
		worker.detach();
	} else if(policy == "steal"){
		std::thread worker = thread(&task_worker, file, std::ref(metadata), job_id, _task_id);
		worker.detach();
	} else {
		
	}
}

//bool TaskManager::destroy_TaskManager(){

//}
	

}
