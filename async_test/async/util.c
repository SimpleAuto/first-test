#include "util.h"
#include "log_fun.h"
#include "log.h"

int log_init_ex(const char* dir,log_lvl_t lvl, uint32_t size, int maxfiles, const char* pre_name, uint32_t logtime)
{
    int ret;
    if(logtime == 0)
    {
        ret = log_init(dir, lvl, size, maxfiles, pre_name);
    }else{
        ret = log_init_t(dir, lvl, pre_name, logtime);
    }

    BOOT_LOG(ret, "Set log dir %s, pre file size %g MB", dir, size / 1024.0 / 1024.0);
}
