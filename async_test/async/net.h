#ifndef _NET_H_
#define _NET_H_

#include <time.h>
#include <stdio.h>

#include "types.h"

enum
{
	//page_size	= 8192
	def_page_size = 8192
};

extern time_t    socket_timeout; 
extern int       page_size;
extern uint32_t  g_send_buf_limit_size;


#endif  // _NET_H_
