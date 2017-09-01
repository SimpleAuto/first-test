#ifndef _TLOG_H
#define _TLOG_H

#define TLOG_BUF_SIZE (8*1024)

void boot_tlog(int ok,int dummy,const char* fmt,...);

#endif
