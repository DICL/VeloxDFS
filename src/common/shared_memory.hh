#pragma once
#include <pthread.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define DEFAULT_KEY 85549

struct shm_buf{
//	pthread_mutexattr_t lock_attr;
	pthread_mutex_t lock;
	uint64_t chunk_size;
	char* buf;
	uint32_t chunk_index;
	uint32_t chunk_offset;
//	uint32_t read_buf_idx;
	bool commit;
};
