#include "tlog.h"
#include "tlog_decl.h"
#include "netutils.h"
#include "fileutils.h"

logger_ctrl_cfg_t logger_ctrl_cfg_struct, *ctrl_cfg = &logger_ctrl_cfg_struct;
logger_svc_info_t logger_svc_info_struct, *svc_info = &logger_svc_info_struct;
logger_udp_sink_t udp_sink_struct       , *udp_sink = &udp_sink_struct;
logger_t          logger_struct         , *logger=&logger_struct;

static int32_t self_thread_pid;

/* 为创建新的 logfile 做准备 */
void init_logfile(logfile_t *logfile)
{
    time_t now = logger->now;
    struct tm *tm = &(logger->tm_now);

    if(ctrl_cfg->time_slice_secs)
        logfile->time_slice_seq = now / ctrl_cfg->time_slice_secs;

    if(tlog_unlikely(logfile->tm_yday != tm->tm_yday))
    {
        logfile->daily_seq = 0;
        logfile->tm_yday = tm->tm_yday;
    }
    /* baselen/basename/infostr/infostrlen 都不能清除 */
    logfile->file_len = 0;
    logfile->fd = -1;
}

/**
 * @param logfile: 当前级别的log文件;
 * @param tm: 用于产生log文件名的时间;
 * @returns -1: failed, >=0: 合法的slice_no
 */
int get_time_slice_no(logfile_t *logfile,char *datedir,struct tm *tm)
{
    char filename[FILENAME_MAX];
    int maxlen = sizeof(filename);

    int no;
    struct stat stbuf;
    for(no=0;no != MAX_SLICE_NO;++no)
    {
        snprintf(filename,maxlen, "%s/%s-%02d%02d%02d-%04d",
                datedir,
                logfile->basename,
                tm->tm_hour,tm->tm_min,tm->tm_sec,no);

        if(stat(filename,&stbuf) == -1)
        {
            if(errno == ENOENT)
                break;
            return -1;
        }

        if(stbuf.st_size < (ctrl_cfg->max_one_size << 20))
            break;
    }
    return no;
}

int gen_timestring(char *buf,int maxlen,logfile_t *log_file,char *datedir)
{
    time_t now = logger->now;
    struct tm *tm = &(logger->tm_now);
    struct tm tm_tmp;
    int32_t slice_no = 0;

    if(ctrl_cfg->time_slice_secs > 0)
    {
        time_t interval = ctrl_cfg->time_slice_secs;
        time_t tmp = now / interval * interval;
        localtime_r(&tmp,&tm_tmp);
        tm = &tm_tmp;
        slice_no = get_time_slice_no(log_file,datedir,&tm_tmp);
    }

    if(slice_no == -1)
        return -1;

    snprintf(buf,maxlen, "%02d%02d%02d-%04d",
            tm->tm_hour,tm->tm_min,tm->tm_sec,slice_no);

    return 0;
}

/**
 * @brief 0: 成功, -1: 失败
 */
int create_new_logfile(logfile_t *logfile)
{
    int fd = -1;
    char timestring[MAX_TIMESTR_LEN];
    char filename[FILENAME_MAX];
    char datedir[FILENAME_MAX];
    struct tm *tm = &(logger->tm_now);

    snprintf(datedir, sizeof(datedir), "%s/%04d%02d%02d", logger->logdir,
            1900+tm->tm_year, 1+tm->tm_mon, tm->tm_mday);
    if (tlog_mkdir_with_parents(datedir, 0755) == -1) {
        fprintf(stderr, "ERROR: failed to mkdir: %s (%d): %s\n",
                datedir, errno, strerror(errno));
        return -1;
    }
    
    /* 到此, 一定要切换文件, 于是先关闭之前的文件 (有的话) */
    if (logfile->fd > 0) {
        close(logfile->fd);
    }
    init_logfile(logfile);

    /* 组织新文件名:
     *  <basename>-<timestring>
     * 其中:
     *  basename: <prefix>-<severity>
     *  timestring: <yyyymmdd>-<HHMMSS>-<slice_no>
     *  infostr: 文件内容第一行: <svcname>-<svrtype>-<hostname> */

    if (gen_timestring(timestring, sizeof(timestring), logfile, datedir) == -1) {
        return -1;
    }
    snprintf(filename, sizeof(filename), "%s/%s-%s",
            datedir, logfile->basename, timestring);

    fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0664);
    if (fd == -1) {
        return -1;
    }
    fcntl(fd, F_SETFD, FD_CLOEXEC);

    logfile->fd = fd;
    /* 写文件头: yyyy-mm-dd HH:MM:SS\t"LOGHEAD"\tpid\tlogfile->infostr */
    char loghead[TLOG_BUF_SIZE];
    int headlen = snprintf(loghead, sizeof(loghead),
            "%04d-%02d-%02d %02d:%02d:%02d\t%s\t%d\t%s\n",
            tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec,
            LOGTYPE_HEAD, self_thread_pid, logfile->infostr);
    write(logfile->fd, loghead, headlen);

    logfile->daily_seq++;

    return 0;
}

/**
 * @brief is_need_shift_file:
 * 判断某类(debug/trace/...)log文件是否需要切换;
 * @returns 0: 不要切换文件, 1: 要切换文件;
 */
int is_need_shift_file(logfile_t *logfile)
{
    time_t now = logger->now;
    struct tm *tm = &(logger->tm_now);
    /* 根据 "每天最大文件数量" 判断要不要切换;
     * 注意: 这里仅仅是对logfile这一类log文件作约束, 例如:
     *  所有类型的log文件, 每天做多能产生多少文件;
     */
    if (ctrl_cfg->daily_max_files
            && logfile->daily_seq >= ctrl_cfg->daily_max_files) {
        return DONT_SHIFT;
    }
    /* 刚初始化 */
    if (logfile->fd == -1) {
        return DO_SHIFT;
    }
    /* 根据 "轮转条件" 判断要不要切换 */
    if (ctrl_cfg->time_slice_secs) {
        int32_t seq = now / ctrl_cfg->time_slice_secs;
        if (tlog_unlikely(seq != logfile->time_slice_seq)) {
            return DO_SHIFT;
        }
    }
    /* 根据 "文件大小" 判断要不要切换 */
    if (tlog_unlikely(ctrl_cfg->max_one_size
                && (logfile->file_len >> 20) >= ctrl_cfg->max_one_size)) {
        return DO_SHIFT;
    }

    /* 根据 "日期" 判断要不要切换 */
    if (tlog_unlikely(logfile->tm_yday != tm->tm_yday)) {
        return DO_SHIFT;
    }

    return DONT_SHIFT;
}

int shift_logger_file(logfile_t *logfile)
{
    /* 如果不需要切换文件, 就立刻返回 */
    if (is_need_shift_file(logfile) == DONT_SHIFT) {
        return 0;
    }

    /* 到此: 一定是要创建新文件, 且 logfile 已经初始化好了 */
    return create_new_logfile(logfile);
}

void boot_tlog(int ok,int dummy,const char* fmt,...)
{
#define SCREEN_COLS 80
#define BOOT_OK   "\e[1m\e[32m[ ok ]\e[m"
#define BOOT_FAIL "\e[1m\e[31m[ failed ]\e[m"
// BOOT_OK [ ok ]  BOOT_FAIL [ failed ]
	int end,i,pos;
	va_list ap;
	
	char buffer[TLOG_BUF_SIZE];
	va_start(ap,fmt);
	end = vsnprintf(buffer, sizeof(buffer)-1 ,fmt ,ap);
	va_end(ap);

	pos = SCREEN_COLS - 10 - (end-dummy) % SCREEN_COLS;
	for(i=0;i<pos;++i)
		buffer[end+i] = ' ';

	buffer[end+i] = '\0';
	
	strcat(buffer,ok==0? BOOT_OK : BOOT_FAIL);
	printf("%s\n",buffer);
	if(ok)
		exit(ok);
}

void write_to_logger(int lvl,const char *logtype,uint32_t uid,uint32_t flag,const char* fmt,...)
{
    if(tlog_unlikely(flag == 0))
        return;
    if(tlog_unlikely(logger->status == logger_status_notinit))
        return;
    if(lvl > ctrl_cfg->log_level)
        return;
    
    // 寻找更好的获取时间的办法
    logger->now = time(NULL);
    struct tm* tm= &(logger->tm_now);
    time_t now = logger->now;
    localtime_r(&now,tm);

    int hlen=9,mlen=0,tot_len=0;
    char logtype_buf[MAX_SVRTYPE_LEN];
    char buffer[TLOG_BUF_SIZE];

    va_list ap;
    va_start(ap,fmt);

    int ret;
    logfile_t *logfile = &(logger->logfile[lvl]);

    /* 构造一行log的格式：
     * yyyy-mm-dd HH:MM:SS\tLOG_TYPE\tUID\tmsg
     */
    if(logtype){
        snprintf(logtype_buf,sizeof(logtype_buf),"%s" ,logtype);
    }else{
        snprintf(logtype_buf,sizeof(logtype_buf),"%s" ,NIL);
    }

    hlen = snprintf(buffer, sizeof(buffer),
            "%04d-%02d-%02d %02d:%02d:%02d\t%s\t%u\t",
            tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,
            tm->tm_hour,tm->tm_min,tm->tm_sec,logtype_buf,uid);

    // 把logline添加到 buffer中的 loghead后面
    mlen = vsnprintf(buffer+hlen,sizeof(buffer)-hlen,fmt,ap);

    // 如果log太长，就在第 8191个字节处截断，并把第8192个字节改成'\n'
    if(tlog_unlikely(mlen >= sizeof(buffer)-hlen))
    {
        mlen = sizeof(buffer) - hlen;
        buffer[sizeof(buffer) - 1] = '\n';
    }

    tot_len = hlen + mlen;
    va_end(ap);

    if(tlog_unlikely(logger->status != logger_status_writing))
    { // 当超过flush time，就开始每次记log都尝试写入
        if((logger->status == logger_status_stop)
            && (now - logger->last_check_diskfull_time >= CHECK_DISKFULL_DUR))
        {
            logger->status = logger_status_writing;
            logger->last_check_diskfull_time = now;
        }
        goto try_udp;
    }

    // 写log
    if(tlog_likely(flag & logger_flag_file))
    { // 检查是否需要更换log文件
        if(shift_logger_file(logfile) < 0){
            goto try_udp;
        }

        // 写入文件
        errno = 0;
        ret = write(logfile->fd,buffer,tot_len);
        if(ret == -1 && errno == ENOSPC && ctrl_cfg->stop_if_diskfull)
        {
            logger->status = logger_status_stop;
            goto try_udp;
        }else{
            logfile->file_len += tot_len;
        }
    }
    
try_udp:
    // 是否需要网络发送log
    if(flag & logger_flag_udp)
    {
       sendlog_to_ubp_sink(buffer,tot_len);
    }
}
