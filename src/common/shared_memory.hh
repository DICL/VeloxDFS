#pragma once

#include <stdio.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <cstdlib>
#include <stdint.h>
#include <unistd.h>

#define DEFAULT_KEY 85549

struct shm_buf{
	uint64_t chunk_size;
	char* buf;
	uint32_t chunk_index;
//	uint32_t chunk_offset;
//	bool commit;
};

struct semaphore {
    pthread_mutex_t lock;
    pthread_cond_t nonzero;
	unsigned head;
	unsigned tail;
	unsigned queue_size;
    unsigned count;
};

struct semaphore * semaphore_create(const char *semaphore_name, unsigned qs);
struct semaphore * semaphore_open(const char *semaphore_name);
void semaphore_close(struct semaphore *semap);

