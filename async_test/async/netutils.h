#ifndef _NETUTILS_H_
#define _NETUTILS_H_

#include "tlog_decl.h"

void sendlog_to_ubp_sink(const char* buffer,int len);
int update_udp_sink_sockaddr(void);

#endif
