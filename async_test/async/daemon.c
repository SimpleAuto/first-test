#include "daemon.h"
#include "config.h"
#include "log.h"
#include "atomic.h"
#include "bindconf.h"

#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <linux/unistd.h>

atomic_t child_pids[max_listen_fds];

int max_fd_num;

volatile int stop        = 0;
volatile int restart     = 0;
volatile int term_signal = 0;

char** saved_argv = NULL;
static char*    arg_start;
static char*    arg_end;
static char*    env_start;
static int      backgd_mode = 0;
static int      status;

const char * const signame[] = 
{
    "SIG_0",
    "SIGHUP",   "SIGINT",   "SIGQUIT",  "SIGILL",
    "SIGTRAP",  "SIGABRT",  "SIGBUS",   "SIGFPE",
    "SIGKILL",  "SIGUSR1",  "SIGSEGV",  "SIGUSR2",
    "SIGPIPE",  "SIGALRM",  "SIGTERM",  "SIGSTKFLT",
    "SIGCHLD",  "SIGCONT",  "SIGSTOP",  "SIGTSTP",
    "SIGTTIN",  "SIGTTOU",  "SIGURG",   "SIGXCPU",
    "SIGXFSZ",  "SIGVTALRM",    "SIGPROF",  "SIGWINCH",
    "SIGIO",    "SIGPWR",   "SIGSYS",
    "SIGRT_0",  "SIGRT_1",  "SIGRT_2",  "SIGRT_3",
    "SIGRT_4",  "SIGRT_5",  "SIGRT_6",  "SIGRT_7",
    "SIGRT_8",  "SIGRT_9",  "SIGRT_10", "SIGRT_11",
    "SIGRT_12", "SIGRT_13", "SIGRT_14", "SIGRT_15",
    "SIGRT_16", "SIGRT_17", "SIGRT_18", "SIGRT_19",
    "SIGRT_20", "SIGRT_21", "SIGRT_22", "SIGRT_23",
    "SIGRT_24", "SIGRT_25", "SIGRT_26", "SIGRT_27",
    "SIGRT_28", "SIGRT_29", "SIGRT_30", "SIGRT_31",
    "SIGRT_32"
};

static void sigterm_handler(int signo)
{
    stop        = 1;
    restart     = 0;
    term_signal = -1;
}

static void signup_handler(int signo)
{
    restart = 1;
    stop    = 1;
}

static void sigchld_handler(int signo, siginfo_t *si, void *p)
{
    pid_t pid = 0;
    while((pid == waitpid(-1, &status,WNOHANG)) > 0)
    {
        int i;
        for(i=0;i< bindconf.bind_num;++i)
        {
            if(atomic_read(&child_pids[i]) == pid)
            {
                atomic_set(&child_pids[i],0);
                break;
            }
        }
    }
}

static inline void dup_argv(int argc,char **argv)
{
    saved_argv = malloc(sizeof(char*) * (argc+1));
    if(!saved_argv)
        return;
    saved_argv[argc] = NULL;
    while(--argc >= 0)
    {
        saved_argv[argc] = strdup(argv[argc]);;
    }
}

static inline void rlimit_reset()
{
    struct rlimit rlim;
    max_fd_num = config_get_intval("max_open_fd",20000);

    rlim.rlim_cur = max_fd_num;
    rlim.rlim_max = max_fd_num;
    if(setrlimit(RLIMIT_NOFILE,&rlim) == -1)
        ALERT_LOG("INIT FD RESOURCE FAILED");

    rlim.rlim_cur = 1 << 30;
    rlim.rlim_max = 1 << 30;
    if(setrlimit(RLIMIT_CORE, &rlim) == -1)
        ALERT_LOG("INIT CORE FILE RESOURCE FAILED");
}

int daemon_start(int argc, char** argv)
{
    // sigset_t 信号集及信号集操作函数：信号集被定义为一种数据类型
    // 与信号阻塞相关函数配合使用
    struct sigaction sa;
    sigset_t sset;
    const char* style;

    // 设置进程所能得到的最大资源的限制
    // rlim_cur 是对资源的软限制
    // rlim_max 是对资源的硬限制
    // 这里限制了core文件大小以及 每个进程能打开的最多文件数(即fd的数量)
    rlimit_reset();

    memset(&sa,0,sizeof(sa));
    signal(SIGPIPE,SIG_IGN);

    sa.sa_handler = sigterm_handler;
    sigaction(SIGINT,&sa,NULL);
    sigaction(SIGTERM,&sa,NULL);
    sigaction(SIGQUIT,&sa,NULL);

    sa.sa_handler = signup_handler;
    sigaction(SIGHUP,&sa,NULL);

    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sa.sa_sigaction = sigchld_handler;
    sigaction(SIGCHLD,&sa,NULL);

    // sigemptyset用来参数set信号集初始化并清空
    sigemptyset(&sset);
    sigaddset(&sset, SIGSEGV);
    sigaddset(&sset, SIGBUS);
    sigaddset(&sset, SIGABRT);
    sigaddset(&sset, SIGILL);
    sigaddset(&sset, SIGCHLD);
    sigaddset(&sset, SIGFPE);
    // 设定对信号屏蔽集内的信号的处理方式
    // SIG_UNBLOCK 从进程屏蔽里将信号删除，非阻塞
    sigprocmask(SIG_UNBLOCK, &sset, &sset);

    arg_start = argv[0];
    arg_end   = argv[argc-1] + strlen(argv[argc-1]) +1;
    env_start = environ[0];
    dup_argv(argc,argv);

    style = config_get_strval("run_mode");
    if(!style || !strcasecmp("background",style))
    {
        daemon(1,1);
        backgd_mode = 1;
        BOOT_LOG(0,"switch to daemon mode");
    }
    return 0;
}

void clean_child_pids()
{
    int i;
    for(i = 0; i < max_listen_fds ;++i)
    {
        atomic_set(&child_pids[i], 0);
    }
}
