#include "util.h"
#include "log_fun.h"
#include "log.h"
#include "config.h"

#include <unistd.h>
#include <fcntl.h>

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

int pipe_create(int pipe_handles[2])
{
    if (pipe (pipe_handles) == -1)
        return -1;

    int rflag, wflag;
    if(config_get_intval("set_pipe_noatime",0) == 1)
    {
        rflag = O_NONBLOCK | O_RDONLY | O_NOATIME;
        wflag = O_NONBLOCK | O_WRONLY | O_NOATIME;
    }else{
        rflag = O_NONBLOCK | O_RDONLY;
        wflag = O_NONBLOCK | O_WRONLY;
    }

    fcntl (pipe_handles[0], F_SETFL, rflag);
    fcntl (pipe_handles[1], F_SETFL, wflag);

    fcntl (pipe_handles[0], F_SETFD, FD_CLOEXEC);
    fcntl (pipe_handles[1], F_SETFD, FD_CLOEXEC);

    return 0;
}
