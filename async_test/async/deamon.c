#include "deamon.h"
#include "config.h"
#include "log.h"

#include <signal.h>
#include <sys/resource.h>

int max_fd_num;

static inline void rlimit_reset()
{
    struct rlimit rlim;
    max_fd_num = config_get_intval("max_open_fd",20000);

    rlim.rlim_cur = max_fd_num;
    rlim.rlim_max = max_fd_num;
    if(setrlimit(RLIMIT_NOFILE,&rlim) == -1)
    {
        ALERT_LOG("INIT FD RESOURCE FAILED");
    }
}

int daemon_start(int argc, char** argv)
{
    // sigset_t 信号集及信号集操作函数：信号集被定义为一种数据类型
    // 与信号阻塞相关函数配合使用
    struct sigaction sa;
    sigset_t sset;
    const char* style;

    rlimit_reset();
    return 0;
}
