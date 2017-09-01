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
}
