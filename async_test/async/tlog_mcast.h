#ifndef _TLOG_MCAST_H_
#define _TLOG_MCAST_H_

#include <stdio.h>
#include "tlog_decl.h"

#define RT_BOOT_TLOG(rt, fmt, args...) \
	do { \
		boot_tlog(rt,0,fmt, ##args); \
		return rt; \
	} while(0)

#define FATAL_TLOG(fmt, args...) \
    do {  \
        write_to_logger(tlog_lvl_fatal,NULL,0,logger_flag_file,"[%s][%d]%s: "fmt"\n",__FILE__,__LINE__,__FUNCTION__,##args ); \
    } while(0)

#endif
