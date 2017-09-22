#ifndef _SHMQ_H_
#define _SHMQ_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>

#include "atomic.h"
#include "bindconf.h"

typedef struct shm_head
{
    volatile int head;
    volatile int tail;
    atomic_t blk_cnt;
} __attribute__ ((packed)) shm_head_t;

typedef struct shm_queue
{
    shm_head_t *addr;
    unsigned int length;
    int pipe_handles[2];
} shm_queue_t;

typedef struct shm_block 
{
	uint32_t	id;
	uint32_t	fd;
	uint32_t	length;	// length of the whole shmblock, including data
	char		type;
	uint8_t		data[];
} __attribute__ ((packed)) shm_block_t;

struct bind_config_elem;
struct bind_config;

int shmq_create(struct bind_config_elem *p);
void close_shmq_pipe(struct bind_config* bc, int idx, int is_child);

#ifdef __cplusplus
}
#endif

#endif
