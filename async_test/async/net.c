#include <stdio.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/types.h>
#include <sys/mman.h>

#include "types.h"
#include "log.h"
#include "list.h"
#include "util.h"
#include "tcp.h"
#include "net.h"

time_t    socket_timeout;
int       page_size;
int       g_listen_port;
char      g_listen_ip[16];
uint32_t  g_send_buf_limit_size;
struct    epinfo epi;

static inline int add_events(int epfd, int fd, uint32_t flag)
{
    struct epoll_event ev;
    ev.events  = flag;
    ev.data.fd = fd;
epoll_add_again:
    if(unlikely(epoll_ctl (epfd,EPOLL_CTL_ADD, fd, &ev)!= 0))
    {
        if(errno == EINTR)
            goto epoll_add_again;
        ERROR_RETURN(("epoll_ctl add %d error: %m",fd),-1);
    }
    return 0;
}

static inline void free_cb(struct conf_buf *p)
{
    if(p->sendptr)
    {
        free(p->sendptr);
        p->sendptr = NULL;
    }
    if(p->recvptr)
    {
        free(p->recvptr);
        p->recvptr = NULL;
    }

    p->sendlen = 0;
    p->recvlen = 0;
}

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

int do_add_conn(int fd, uint8_t type, struct sockaddr_in* peer, bind_config_elem_t* bc_elem)
{
    static uint32_t seq = 0;
    uint32_t flag;
    
    switch(type)
    {
        case fd_type_pipe:
        case fd_type_mcast:
        case fd_type_addr_mcast:
        case fd_type_udp:
            flag = EPOLLIN;
            break;
        case fd_type_asyn_connect:
            flag = EPOLLIN;
            break;
        default:
            flag = EPOLLIN | EPOLLET;
            break;
    }

    if(add_events(epi.epfd, fd, flag) == -1)
        return -1;

    memset(&epi.fds[fd], 0x0, sizeof(struct fdinfo));
    epi.fds[fd].sockefd = fd;
    epi.fds[fd].type    = type;
    epi.fds[fd].id      = ++seq;
    if(seq == 0)
        epi.fds[fd].id = ++seq;
    if(peer)
    {
        epi.fds[fd].sk.remote_ip   = peer->sin_addr.s_addr;
        epi.fds[fd].sk.remote_port = peer->sin_port;
    }
    epi.fds[fd].bc_elem = bc_elem;
    epi.maxfd   =  epi.maxfd > fd ? epi.maxfd : fd;
    epi.count++;

    TRACE_LOG("add fd=%d, type=%d, id=%u", fd, type, epi.fds[fd].id);
    return 0;
}

int net_start(const char* listen_ip, in_port_t listen_port, bind_config_elem_t* bc_elem)
{
    int ret_code = -1;
    g_listen_port = listen_port;
    
    int listenfd = safe_socket_listen(listen_ip, listen_port, SOCK_STREAM, 1024, 32 * 1024);
    if( listenfd != -1)
    {
        set_io_blockability(listenfd,1);
        do_add_conn(listen_port,fd_type_listen,0,bc_elem);
        ret_code = 0;
    }

    BOOT_LOG(ret_code, "Listen on %s:%u", listen_ip ? listen_ip: "ANYADDR",listen_port);
}

void net_exit()
{
    for(int i=0; i< epi.maxfd+1 ; ++i)
    {
        if(epi.fds[i].type == fd_type_unused)
            continue;

        free_cb(&epi.fds[i].cb);
        close(i);
    }

    free (epi.fds);
    free (epi.evs);
    close(epi.epfd);
}
