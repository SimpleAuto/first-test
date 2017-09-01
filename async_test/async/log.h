#ifndef _LOG_H
#define _LOG_H

#include "tlog.h"


#define RT_BOOT_TLOG(rt, fmt, args...) \
	do { \
		boot_tlog(rt,0,fmt, ##args); \
		return rt; \
	} while(0)

#ifdef BOOT_LOG
#undef BOOT_LOG
#endif
#define BOOT_LOG(OK, fmt, args...) RT_BOOT_TLOG(OK, fmt,##args)

#endif
