#ifndef _UTIL_H_
#define _UTIL_H_

#include "tlog_decl.h"
#include "log_fun.h"

int log_init_ex(const char* dir,log_lvl_t lvl, uint32_t size, int maxfiles, const char* pre_name, uint32_t logtime);

#endif // _UTIL_H_
