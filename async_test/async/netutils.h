#ifndef _NETUTILS_H_
#define _NETUTILS_H_

#include "tlog_decl.h"

void sendlog_to_ubp_sink(const char* buffer,int len);
void do_send_overflow(void);
int update_udp_sink_sockaddr(void);
int init_logger_udp_sink_sendfd(void);

#endif
