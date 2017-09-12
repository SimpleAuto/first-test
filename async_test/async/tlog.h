#ifndef _TLOG_H
#define _TLOG_H

#include <stdint.h>
#include "tlog_decl.h"
#include "tlog_mcast.h"

#define INIT_DEFAULT_LOGGER_SYSTEM(_logdir, _pfx, _svc, _gameid, _svrtp) \
    do { \
        int _ret; \
        _ret = init_logger_ctrl_cfg(def_log_level,def_daily_max_files, \
                def_max_one_size, def_stop_if_diskfull, def_time_slice_secs);  \
        if( _ret < 0) { fprintf(stderr,"Failed to init ctrl_cfg\n"); exit(-1);} \
        _ret = init_logger_svc_info(_svc, _gameid, _svrtp); \
        if( _ret < 0) { fprintf(stderr,"Failed to init svc_info\n"); exit(-1);} \
        _ret = init_logger(_logdir , _pfx); \
        if( _ret < 0) { fprintf(stderr,"Failed to init logger\n"); exit(-1);} \
    } while(0)

int init_logger_ctrl_cfg(int lvl, int daily_max_files, size_t max_one_size, int stop_if_diskfull,  int time_slice_secs); 
int init_logger_svc_info(const char* svcname, int gameid, const char* svrtype);
int init_logger(const char* dir, const char* prefix);
void boot_tlog(int ok,int dummy,const char* fmt,...);
void write_to_logger(int lvl,const char *logtype,uint32_t uid,uint32_t flag,const char* fmt,...);
void init_logfile(logfile_t *logfile);
int shift_logger_file(logfile_t *logfile);
int create_new_logfile(logfile_t *logfile);
int is_need_shift_file(logfile_t *logfile);
int gen_timestring(char *buf,int maxlen,logfile_t *log_file,char *datedir);
int get_time_slice_no(logfile_t *logfile,char *datedir,struct tm *tm);

#endif
