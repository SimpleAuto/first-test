#ifndef _TLOG_H
#define _TLOG_H

#include <stdint.h>
#include "tlog_decl.h"
#include "tlog_mcast.h"

void boot_tlog(int ok,int dummy,const char* fmt,...);
void write_to_logger(int lvl,const char *logtype,uint32_t uid,uint32_t flag,const char* fmt,...);
void init_logfile(logfile_t *logfile);
int shift_logger_file(logfile_t *logfile);
int create_new_logfile(logfile_t *logfile);
int is_need_shift_file(logfile_t *logfile);
int gen_timestring(char *buf,int maxlen,logfile_t *log_file,char *datedir);
int get_time_slice_no(logfile_t *logfile,char *datedir,struct tm *tm);

#endif
