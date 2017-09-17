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

#ifdef INFO_LOG
#undef INFO_LOG
#endif
#define INFO_LOG(fmt, args...) INFO_TLOG(fmt, ##args)

#ifdef KINFO_LOG
#undef KINFO_LOG
#endif
#define KINFO_LOG(key, fmt, args...) INFO_TLOG("%u "fmt, key, ##args)

#ifdef DEBUG_LOG
#undef DEBUG_LOG
#endif
#define DEBUG_LOG(fmt, args...) DEBUG_TLOG(fmt, ##args)

#ifdef KDEBUG_LOG
#undef KDEBUG_LOG
#endif
#define KDEBUG_LOG(key, fmt, args...) DEBUG_TLOG("%u "fmt, key, ##args)

#ifdef TRACE_LOG
#undef TRACE_LOG
#endif
#define TRACE_LOG(fmt, args...) TRACE_TLOG(fmt, ##args)

#ifdef KTRACE_LOG
#undef KTRACE_LOG
#endif
#define KTRACE_LOG(key, fmt, args...) TRACE_TLOG("%u "fmt, key, ##args)

#endif
