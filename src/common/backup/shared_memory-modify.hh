#pragma once
#include <pthread.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <cstdlib>


#define DEFAULT_KEY 85549

struct shm_buf{
	uint64_t chunk_size;
	char* buf;
	uint32_t chunk_index;
	uint32_t chunk_offset;
	bool commit;
};

struct semaphore {
    pthread_mutex_t lock;
    pthread_cond_t nonzero;
	unsigned head;
	unsigned tail;
	unsigned queue_size;
    unsigned count;
};

typedef struct semaphore semaphore_t;

semaphore_t *
semaphore_create(char *semaphore_name, unsigned qs)
{
	int fd;
    semaphore_t *semap;
    pthread_mutexattr_t psharedm;
    pthread_condattr_t psharedc;

    fd = open(semaphore_name, O_RDWR | O_CREAT, 0666);
    if (fd < 0){
		printf("%s : fd open failed\n", semaphore_name);
        return (NULL);
	}
    (void) ftruncate(fd, sizeof(semaphore_t));
    (void) pthread_mutexattr_init(&psharedm);
    (void) pthread_mutexattr_setpshared(&psharedm,
        PTHREAD_PROCESS_SHARED);
    (void) pthread_condattr_init(&psharedc);
    (void) pthread_condattr_setpshared(&psharedc,
        PTHREAD_PROCESS_SHARED);
    semap = (semaphore_t *) mmap(NULL, sizeof(semaphore_t),
            PROT_READ | PROT_WRITE, MAP_SHARED,
            fd, 0);
	
    close (fd);
    (void) pthread_mutex_init(&semap->lock, &psharedm);
    (void) pthread_cond_init(&semap->nonzero, &psharedc);
	semap->head = 0;
	semap->tail = 0;
	semap->queue_size = qs;
    semap->count = 0;
    return (semap);
}

semaphore_t *
semaphore_open(char *semaphore_name)
{
    int fd;
    semaphore_t *semap;

    fd = open(semaphore_name, O_RDWR, 0666);
    if (fd < 0)
        return (NULL);
    semap = (semaphore_t *) mmap(NULL, sizeof(semaphore_t),
            PROT_READ | PROT_WRITE, MAP_SHARED,
            fd, 0);
    close (fd);
    return (semap);
}

void
semaphore_close(semaphore_t *semap)
{
    munmap((void *) semap, sizeof(semaphore_t));
}
