#ifndef _NET_H_
#define _NET_H_

#include <time.h>
#include <stdio.h>
#include <sys/epoll.h>

#include "types.h"
#include "bindconf.h"
#include "list.h"

enum
{
	//page_size	= 8192
	def_page_size_ = 8192
};

enum 
{
	fd_type_unused = 0,
	fd_type_listen,
	fd_type_pipe,
	fd_type_remote,
	fd_type_mcast,
	fd_type_addr_mcast,
	fd_type_udp,
	fd_type_asyn_connect
};

extern time_t    socket_timeout; 
extern int       page_size;
extern int       g_listen_port;
extern char      g_listen_ip[16];
extern uint32_t  g_send_buf_limit_size;

typedef struct conf_buf
{
    uint32_t   rcvprotlen;
    uint32_t   recvlen;
    uint32_t   sendlen;
    uint32_t   sndbufsz;
    uint8_t*   recvptr;
    uint8_t*   sendptr;
}conf_buf_t;

typedef struct skinfo
{
    uint16_t    remote_port;
    uint32_t    remote_ip;
    uint32_t    last_tm;
} __attribute__((packed)) skinfo_t;

struct bind_config_elem;

typedef struct fdinfo{
    uint32_t    id;
    int         sockefd;
    uint8_t     type;
    uint8_t     flag;
    conf_buf_t  cb;
    skinfo_t    sk;
    struct bind_config_elem* bc_elem;
    
    void        (*callback)(int fd, void *arg);
    void*       arg;
    list_head_t list;
}fdinfo_t;

struct epinfo
{
    fdinfo_t*           fds;
    struct epoll_event* evs;
    list_head_t         close_head;
    list_head_t         etin_head;
    int                 epfd;
    int                 maxfd;
    int                 max_ev_num;
    int                 count;
};

int net_init(int size, int maxevents);
int do_add_conn(int fd, uint8_t type, struct sockaddr_in* peer, bind_config_elem_t* bc_elem);

#endif  // _NET_H_
