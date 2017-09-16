#ifndef _LOG_H
#define _LOG_H

#include "tlog.h"
#include "tlog_mcast.h"

#ifdef ALERT_LOG
#undef ALERT_LOG
#endif 
#define ALERT_LOG(fmt, args...) FATAL_TLOG(fmt,##args)

#ifdef BOOT_LOG
#undef BOOT_LOG
#endif
#define BOOT_LOG(OK, fmt, args...) RT_BOOT_TLOG(OK, fmt,##args)

#ifdef ERROR_LOG
#undef ERROR_LOG
#endif
#define ERROR_LOG(fmt, args...) ERROR_TLOG(fmt, ##args)

#endif
