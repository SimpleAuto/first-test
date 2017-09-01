#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <stdarg.h>
#include <assert.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include "tlog.h"
#include "tlog_decl.h"
#include "netutils.h"

logger_ctrl_cfg_t logger_ctrl_cfg_struct, *ctrl_cfg = &logger_ctrl_cfg_struct;
logger_svc_info_t logger_svc_info_struct, *svc_info = &logger_svc_info_struct;
logger_udp_sink_t udp_sink_struct       , *udp_sink = &udp_sink_struct;
logger_t          logger_struct         , *logger=&logger_struct;

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

    int ret;
    int hlen=9,mlen=0,tot_len=0;
    char logtype_buf[MAX_SVRTYPE_LEN];
    char buffer[TLOG_BUF_SIZE];

    va_list ap;
    va_start(ap,fmt);

    logfile_t *log_file = &(logger->logfile[lvl]);

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
        if(shift_)
    }
    
try_udp:
    // 是否需要网络发送log
    if(flag & logger_flag_udp)
    {
       sendlog_to_ubp_sink(buffer,tot_len);
    }
}
