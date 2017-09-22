#ifndef _BINDCONF_H_
#define _BINDCONF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <netinet/in.h>
#include "types.h"
#include "shmq.h"

enum 
{
    max_listen_fds  = 500 
};

typedef struct bind_config_elem {
    uint32_t    online_id;
    char        online_name[16];
    char        bind_ip[16];
    in_port_t   bind_port;
    uint8_t     restart_cnt;
    shm_queue_t sendq;
    shm_queue_t recvq;
}bind_config_elem_t;

typedef struct bind_config 
{
    int                 bind_num;
    bind_config_elem_t  configs[max_listen_fds];
}bind_config_t;

extern bind_config_t bindconf;

int load_bind_file(const char* file_name);

#ifdef __cplusplus
}
#endif

#endif
