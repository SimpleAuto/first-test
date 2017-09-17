#include<stdio.h>
#include<time.h>

#include "net.h"
#include "types.h"

time_t    socket_timeout;
int       page_size;
int       g_listen_port;
char      g_listen_ip[16];
uint32_t  g_send_buf_limit_size;
