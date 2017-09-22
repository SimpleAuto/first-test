#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "net.h"
#include "types.h"
#include "log.h"
#include "list.h"

time_t    socket_timeout;
int       page_size;
int       g_listen_port;
char      g_listen_ip[16];
uint32_t  g_send_buf_limit_size;

struct epinfo epi;

int net_init(int size, int maxevents)
{
    if ( (epi.epfd = epoll_create(maxevents)) < 0)
    {
        ERROR_LOG("epoll_create failed, %s", strerror(errno));
        return -1;
    }

    epi.evs = calloc(maxevents , sizeof(struct epoll_event)); 
    if(!epi.evs)
        goto events_fail;

    epi.fds = (struct fdinfo*) calloc (size, sizeof(struct fdinfo));
    if(!epi.fds)
        goto fd_fail;

    epi.max_ev_num = maxevents;
    epi.maxfd      = 0;
    epi.count      = 0;
    INIT_LIST_HEAD(&epi.etin_head);
    INIT_LIST_HEAD(&epi.close_head);
    return 0;

fd_fail:
    free (epi.evs);

events_fail:
    close(epi.epfd);
    ERROR_RETURN (("malloc failed,size=%d",size), -1);
}
