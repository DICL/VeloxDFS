#pragma once
#include "shared_memory.hh"

struct semaphore * semaphore_create(const char *semaphore_name, unsigned qs)
{
	int fd;
    struct semaphore *semap;
    pthread_mutexattr_t psharedm;
    pthread_condattr_t psharedc;

    fd = open(semaphore_name, O_RDWR | O_CREAT, 0666);
    if (fd < 0){
		printf("%s : fd open failed\n", semaphore_name);
        return (NULL);
	}
    (void) ftruncate(fd, sizeof(struct semaphore));
    (void) pthread_mutexattr_init(&psharedm);
    (void) pthread_mutexattr_setpshared(&psharedm,
        PTHREAD_PROCESS_SHARED);
    (void) pthread_condattr_init(&psharedc);
    (void) pthread_condattr_setpshared(&psharedc,
        PTHREAD_PROCESS_SHARED);
    semap = (struct semaphore *) mmap(NULL, sizeof(struct semaphore),
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

struct semaphore * semaphore_open(const char *semaphore_name)
{
    int fd;
    struct semaphore *semap;

    fd = open(semaphore_name, O_RDWR, 0666);
    if (fd < 0)
        return (NULL);
    semap = (struct semaphore *) mmap(NULL, sizeof(struct semaphore),
            PROT_READ | PROT_WRITE, MAP_SHARED,
            fd, 0);
    close (fd);
    return (semap);
}

void semaphore_close(struct semaphore *semap)
{
    munmap((void *) semap, sizeof(struct semaphore));
}

