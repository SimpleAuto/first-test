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

#define RT_ERROR_TLOG(rt, fmt, args...) \
    do {  \
        write_to_logger(tlog_lvl_error, NULL, 0, logger_flag_file, "[%s][%d]%s: "formt"\n",  __FILE__, __LINE__, __FUNCTION__, ##args);  \
        return rt; \
    }while (0)

#define ERROR_TLOG(fmt, args...) \
	do { \
		write_to_logger(tlog_lvl_error, NULL, 0, logger_flag_file, "[%s][%d]%s: "fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##args); \
	} while (0)

// ctrl_interfaces
#define SET_LOG_LEVEL(lvl) \
	do { \
		if ((lvl >= tlog_lvl_min) && (lvl < tlog_lvl_max)) { \
			ctrl_cfg->log_level = lvl; \
		} \
	} while (0)

#define SET_DAILY_MAX_FILES(max) \
	do { \
		if (max >= 0 && max <= DAILY_MAX_FILES) { \
			ctrl_cfg->daily_max_files = max; \
		} \
	} while (0)

#define SET_MAX_ONE_SIZE(max) \
	do { \
		if (max >= MIN_ONE_LOG_SIZE && max <= MAX_ONE_LOG_SIZE) { \
			ctrl_cfg->max_one_size = max; \
		} \
	} while (0)

#define SET_STOP_IF_DISKFULL(onoff) \
	do { \
		if (onoff == 0 || onoff == 1) { \
			ctrl_cfg->stop_if_diskfull = onoff; \
		} \
	} while (0)

#define SET_TIME_SLICE_SECS(secs) \
	do { \
		if (secs >= 0) { \
			ctrl_cfg->time_slice_secs = secs; \
		} \
	} while (0)

#endif // _TLOG_MCAST_H_
