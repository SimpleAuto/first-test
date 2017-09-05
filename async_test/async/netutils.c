#include "netutils.h"
#include "tlog_decl.h"

extern logger_t          *logger;
extern logger_udp_sink_t *udp_sink;
extern logger_svc_info_t *svc_info;

int update_udp_sink_sockaddr(void)
{
    char *ip = udp_sink->ip;
    uint16_t port = udp_sink->port;

    udp_sink->toaddr.sin_family = AF_INET;
    udp_sink->toaddr.sin_port   = htons(port);
    if(inet_pton(AF_INET,ip,&(udp_sink->toaddr.sin_addr)) != 1)
    {
        perror("inet_pton: ");
        return -1;
    }
    return 0;
}

int init_logger_udp_sink_sendfd(void)
{
    if(udp_sink->sendfd >= 0)
        close(udp_sink->sendfd);

    if((udp_sink->sendfd = socket(PF_INET,SOCK_DGRAM,0)) == -1)
    {
        perror("socket: ");
        return -1;
    }
    return 0;
}

void do_send_overflow(void)
{
    /* 不关心发送成功还是失败 */
    static char buffer[1024] = "[SYSTEM]\ttrace_log is too heavy!\n";
    size_t len = strlen(buffer);
    socklen_t tolen = sizeof(udp_sink->toaddr);

    sendto(udp_sink->sendfd, buffer, len, 0,
            (const struct sockaddr *)(&udp_sink->toaddr), tolen);
}

void sendlog_to_ubp_sink(const char* buffer,int len)
{
    time_t now = logger->now;
    // 0 在上次传输过程中超过了流量上限，还没有发送通知
    // 1 已经发送了通知
    static int    is_send_overflow = 0;
    static time_t last_init_time   = 0;

    if(tlog_unlikely(udp_sink->addr_changed))
    {
        update_udp_sink_sockaddr();
        udp_sink->addr_changed = 0;
    }

    if(tlog_unlikely((udp_sink->sendfd == -1)
            && (now-last_init_time >= MIN_WAIT_UDP_SINK_TIME)))
    {
        last_init_time = now;
        if(init_logger_udp_sink_sendfd() < 0)
            return ;
    }
    
    /* 流量控制, 每秒往令牌桶里增加固定的令牌数量 */
    if(now != udp_sink->rate_limit.last_rate_time)
    { // 要用到 limit 时才考虑limit是不是要更新
        if(tlog_unlikely(udp_sink->rate_limit.chg_limit))
        {
            udp_sink->rate_limit.limit = udp_sink->rate_limit.chg_limit;
            udp_sink->rate_limit.chg_limit = 0;
        }
        // 注意: 是直接赋值, 而不是加
        udp_sink->rate_limit.token = udp_sink->rate_limit.limit;
        udp_sink->rate_limit.last_rate_time = now;
        is_send_overflow = 0;
    }

    if(len > udp_sink->rate_limit.token)
    {
        /* token 已经不够了 */
        if (!is_send_overflow) {
            /* 就算 limit 配成了 0, 导致每次都进入token不够的分支,
             * 极限也是每秒发送一条overflow */
            do_send_overflow();
            is_send_overflow = 1;
        }
        return ;
    }

    socklen_t tolen = sizeof(udp_sink->toaddr);
    int ret = sendto(udp_sink->sendfd, buffer, len, 0,
            (const struct sockaddr *)(&udp_sink->toaddr), tolen);
    if (ret == -1) {
        close(udp_sink->sendfd);
        udp_sink->sendfd = -1;
    }else{
        udp_sink->rate_limit.token -= ret;
    }
}
