#ifndef _SHMQ_H_
#define _SHMQ_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "atomic.h"
#include <stdint.h>

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
#ifdef __cplusplus
}
#endif

#endif
