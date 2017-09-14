#include "log_fun.h"
#include "types.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>

#define MAX_LOG_CNT (10000000)

static log_lvl_t   log_level  = log_lvl_debug;     // 缺省的日志等级
static log_dest_t  g_log_dest = log_dest_terminal; // 将日志输出到屏幕上
static int         max_log_files;                  // 日志最大的文件数
static int         config_max_log_files;           // 保存原有的max_log_files
static int         g_logtime_interval;             // 每个日志文件记录日志的总时间(秒)
static int         has_init;
static uint32_t    log_size;                       // 日志大小
static char        log_dir[256];                   // log的路径
static char        log_pre[32];                    // log文件名的前缀
//static const char* color_end = "\e[m";
//static pthread_mutex_t g_shift_fd_mutex = PTHREAD_MUTEX_INITIALIZER;
//static const char* log_color[] = 
//{
//	"\e[1m\e[35m", "\e[1m\e[33m", "\e[1m\e[34m", "\e[1m\e[31m",
//	"\e[1m\e[32m", "\e[1m\e[36m", "\e[1m\e[1m", "\e[1m\e[37m",	"\e[1m\e[37m"
//};

static struct fds_t 
{
	int		seq;
	int		opfd;
	int		day;
	char	base_filename[64];
	int		base_filename_len;
	int  	cur_day_seq_count;//当天轮转文件的个数
} fds_info[log_lvl_max];

enum 
{
	log_buf_sz	= 8192
};

static const char* lognames[] = 
{ 
    "emerg", "alert", "crit", "error",
    "warn", "notice", "info", "debug", "trace" 
};

static inline void log_file_path(int lvl, int seq, char *file_name, const struct tm *tm)
{
    assert((lvl >= log_lvl_emerg) && (lvl <= log_lvl_trace));
    if(g_logtime_interval)
    {
        time_t t = time(NULL) / g_logtime_interval * g_logtime_interval;
        struct tm tmp_tm;
        localtime_r(&t, &tmp_tm);
        sprintf(file_name, "%s/%s%04d%02d%02d%02d%02d", log_dir, fds_info[lvl].base_filename, 
                tmp_tm.tm_year + 1900, tmp_tm.tm_mon + 1, tmp_tm.tm_mday, tmp_tm.tm_hour, tmp_tm.tm_min);
    }else{
        sprintf(file_name, "%s/%s%04d%02d%02d%07d",  log_dir, fds_info[lvl].base_filename,
                tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, seq);
    }
}

static inline void log_file_name(int lvl, int seq, char *file_name, const struct tm *tm)
{
    assert((lvl >= log_lvl_emerg) && (lvl <= log_lvl_trace));
    sprintf(file_name, "%s%04d%02d%02d%07d", fds_info[lvl].base_filename, tm->tm_year + 1900, tm->tm_mon + 1 , tm->tm_mday , seq);
}

static inline int  get_logfile_seqno(const char* filename, int loglvl)
{
    return atoi(&filename[fds_info[loglvl].base_filename_len + 8]);
}

static int get_log_seq_recycle(int lvl)
{
    char file_name[FILENAME_MAX] = {0};

    DIR* dir = opendir(log_dir);
    if(!dir)
        return -1;

    struct dirent *dentry;
    while((dentry = readdir(dir)))
    {
        if((strncmp(dentry->d_name, fds_info[lvl].base_filename , fds_info[lvl].base_filename_len) == 0)
                && (strcmp(dentry->d_name, file_name) > 0))
        {
            snprintf(file_name, sizeof(file_name), "%s" , dentry->d_name);
        }
    }
    closedir(dir);
    
    struct tm tm;
    time_t now = time(NULL);
    localtime_r(&now,&tm);

    if(file_name[0] == '\0')
        log_file_name(lvl, 0, file_name, &tm);

    char *date = &file_name[fds_info[lvl].base_filename_len];
    char today[9];
    int seqno = get_logfile_seqno(file_name, lvl);

    snprintf(today, sizeof(today), "%04d%02d%02d", tm.tm_year + 1900, tm.tm_mon + 1 , tm.tm_mday);
    if(strncmp(today, date, 8))
        ++seqno;

    return seqno % max_log_files;
}

static int get_log_seq_nonrecycle(int lvl)
{
    char file_name[FILENAME_MAX] = {0};

    struct tm tm;
    time_t now = time(NULL);
    localtime_r(&now,&tm);

    int seq;
    for(seq = 0; seq != MAX_LOG_CNT; ++seq)
    {
        log_file_path(lvl, seq, file_name, &tm);
        if(access(file_name , F_OK) == -1)
            break;
    }

    return (seq ? ( seq - 1 ): 0);
}

static inline int request_log_seq(int lvl)
{
    return (max_log_files ? get_log_seq_recycle(lvl) : get_log_seq_nonrecycle(lvl));
}

static inline int get_log_time()
{
    return time(NULL) / g_logtime_interval;
}

int log_init_t(const char* dir, log_lvl_t lvl,const char* pre_name,int logtime)
{
    assert((logtime > 0) && (logtime <= 30000000));
    g_logtime_interval = logtime * 60;

    return log_init(dir, lvl, 0, 0, pre_name);
}

int log_init(const char* dir, log_lvl_t lvl, uint32_t size, int maxfiles, const char* pre_name)
{
    assert((maxfiles >=0) && (maxfiles <= MAX_LOG_CNT));
    log_level = lvl;
    if(!dir || (strlen(dir) == 0))
        return 0;

    int i,ret_code = -1;
    log_size      = size;
    max_log_files = maxfiles;
    config_max_log_files = maxfiles;
    if((lvl < log_lvl_emerg) || (lvl > log_lvl_trace))
    {
        fprintf(stderr, "init log error , invalid log level=%d\n",lvl);
        goto exit_entry;
    }

    // log file 最大不能超过2G
    if(size > (1<<31))
    {
        fprintf(stderr, "init log error , invalid log level=%d\n", size);
        goto exit_entry;
    }

    if(access(dir ,W_OK))
    {
        fprintf(stderr, "access log dir %s error, %m\n",dir);
        goto exit_entry;
    }

    strncpy(log_dir, dir, sizeof(log_dir) -1);
    if(pre_name != NULL)
        strncpy(log_pre, pre_name, sizeof(log_pre)-1);

    for(i = log_lvl_emerg; i <= log_lvl_trace; ++i)
    {
        fds_info[i].base_filename_len = snprintf(fds_info[i].base_filename, sizeof(fds_info[i].base_filename), "%s%s", log_pre, lognames[i]);
        fds_info[i].opfd              = -1;
        fds_info[i].seq               = g_logtime_interval ? get_log_time() : request_log_seq(i);
        if(fds_info[i].seq < 0)
            goto exit_entry;
    }

    g_log_dest  = log_dest_file;
    has_init    = 1;
    ret_code    = 0;

exit_entry:
    return ret_code;
}
