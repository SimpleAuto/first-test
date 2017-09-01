#ifndef _TLOG_H
#define _TLOG_H

#include <stdint.h>

#define TLOG_BUF_SIZE (8*1024)

void boot_tlog(int ok,int dummy,const char* fmt,...);
void write_to_logger(int lvl,const char *logtype,uint32_t uid,uint32_t flag,const char* fmt,...);

#endif
