#ifndef _UTIL_H_
#define _UTIL_H_

#include "tlog_decl.h"
#include "log_fun.h"

#ifdef likely
#undef likely
#endif
#define likely(x) __builtin_expect(!!(x), 1)

#ifdef unlikely
#undef unlikely
#endif
#define unlikely(x) __builtin_expect(!!(x), 0)

int log_init_ex(const char* dir,log_lvl_t lvl, uint32_t size, int maxfiles, const char* pre_name, uint32_t logtime);

int pipe_create(int pipe_handles[2]);

#endif // _UTIL_H_
