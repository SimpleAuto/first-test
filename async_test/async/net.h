#ifndef _NET_H_
#define _NET_H_

#include <time.h>
#include <stdio.h>

#include "types.h"

enum
{
	//page_size	= 8192
	def_page_size_ = 8192
};

extern time_t    socket_timeout; 
extern int       page_size;
extern int       g_listen_port;
extern char      g_listen_ip[16];
extern uint32_t  g_send_buf_limit_size;


#endif  // _NET_H_
