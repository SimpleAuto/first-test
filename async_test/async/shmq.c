/*
 *************************************************************************
 *    File Name:  shmq.c
 *       Author: Thomas Shanghai,Inc.
 *         Mail: 982737593@qq.com
 * Created Time: 2017年09月22日 星期五 15时38分52秒
 * 
 *  Description: ...
 * 
 ************************************************************************
*/
#include <stdio.h>
#include <sys/mman.h>

#include "bindconf.h"
#include "config.h"
#include "atomic.h"
#include "util.h"
#include "log.h"
#include "shmq.h"

static int do_shmq_create(struct shm_queue *q)
{
    q->addr = (shm_head_t *) mmap( NULL, q->length, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1 , 0);
    if(q->addr == MAP_FAILED)
        ERROR_RETURN (("mmap failed, %s", strerror(errno)), -1);
    
    q->addr->head = sizeof(shm_head_t);
    q->addr->tail = sizeof(shm_head_t);
    atomic_set (&(q->addr->blk_cnt), 0);
    pipe_create (q->pipe_handles);
    return 0;
}

int shmq_create(bind_config_elem_t *p)
{
    int err;
    p->sendq.length = config_get_intval("shmq_length", 1 << 26);
    p->recvq.length = p->sendq.length;

    err = do_shmq_create(&(p->sendq)) | do_shmq_create(&(p->recvq));
    BOOT_LOG(err, "Create shared memory queue: %dMB", p->recvq.length / 1024 / 512);
}

void close_shmq_pipe(bind_config_t* bc, int idx, int is_child)
{
    if(is_child)
    {
        int i=0;
        for(;i != idx ; ++i)
        {
            close(bc->configs[i].recvq.pipe_handles[1]);
            close(bc->configs[i].sendq.pipe_handles[0]);
        }
    }else{
        close(bc->configs[idx].recvq.pipe_handles[1]);
        close(bc->configs[idx].sendq.pipe_handles[0]);
    }
}
