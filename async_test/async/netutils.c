#include "netutils.h"
#include "tlog_decl.h"
#include <fcntl.h>
#include <unistd.h>

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

int tlog_set_socket_nonblock(int sock)
{
    // fcntl 改变已打开文件的性质，可以针对描述符提供控制
    // rg的最小且仍未使用的文件描述符，并且复制参数fd的文件描述符。执行成功则返回新复制的文件描述符。新描述符与fd共享同一文件表项，但是新描述符有它自己的一套文件描述符标志，其中FD_CLOEXEC文件描述符标志被清除。请参考dup2()。
    // F_GETFD取得close-on-exec旗标。若此旗标的FD_CLOEXEC位为0，代表在调用exec()相关函数时文件将不会关闭。
    // F_SETFD 设置close-on-exec 旗标。该旗标以参数arg 的FD_CLOEXEC位决定。
    // F_GETFL 取得文件描述符状态旗标，此旗标为open（）的参数flags。
    // F_SETFL 设置文件描述符状态旗标，参数arg为新旗标，但只允许O_APPEND、O_NONBLOCK和O_ASYNC位的改变，其他位的改变将不受影响。
    // F_GETLK 取得文件锁定的状态。
    // F_SETLK 设置文件锁定的状态。此时flcok 结构的l_type 值必须是F_RDLCK、F_WRLCK或F_UNLCK。如果无法建立锁定，则返回-1，错误代码为EACCES 或EAGAIN。
    // F_SETLKW F_SETLK 作用相同，但是无法建立锁定时，此调用会一直等到锁定动作成功为止。若在等待锁定的过程中被信号中断时，会立即返回-1，错误代码为EINTR。
    int flag;
    flag  = fcntl(sock, F_GETFL,0);
    flag |= O_NONBLOCK;
    flag |= O_NDELAY;
    fcntl(sock, F_SETFL, flag);
    return 0;
}

int do_set_trace_uid(int fd, time_t now,logger_ctrl_pkg_t *pkg, struct sockaddr_in *from,socklen_t fromlen)
{
    return 0;
}

int do_set_trace_addr(int fd, time_t now,logger_ctrl_pkg_t *pkg, struct sockaddr_in *from,socklen_t fromlen)
{
    return 0;
}

int do_set_rate_limit(int fd, time_t now,logger_ctrl_pkg_t *pkg, struct sockaddr_in *from,socklen_t fromlen)
{
    return 0;
}

void tlog_set_socket_reuseaddr(int sock)
{
    // 参考　setsockopt_explain.txt
    int flag = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
}

int tlog_mcast_join(int mfd, const struct sockaddr* grpaddr,socklen_t grpaddrlen,const char *ethx)
{
    struct group_req req = {0};
    assert(grpaddrlen <= sizeof(req.gr_group));

    // if_nametoindex 名字和索引之间转换
    req.gr_interface = if_nametoindex(ethx);
    if(req.gr_interface == 0)
    {
        errno = ENXIO;
        return -1;
    }

    memcpy(&req.gr_group,grpaddr,grpaddrlen);
    return setsockopt(mfd, IPPROTO_IP, MCAST_JOIN_GROUP, &req, sizeof(req));
}

void proc_logger_pkg(int mfd, time_t now, char *recvbuf, ssize_t len, struct sockaddr_in *from, socklen_t fromlen)
{
    logger_ctrl_pkg_t *pkg = (logger_ctrl_pkg_t*)recvbuf;

    pkg->len = ntohl(pkg->len);
    pkg->cmd = ntohl(pkg->cmd);

    if( pkg->len != len)
        return;

    switch(pkg->cmd)
    {
        case lc_set_trace_uid:
            do_set_trace_uid(mfd,now,pkg,from,fromlen);
            break;

        case lc_unset_trace_uid:
            break;

        case lc_set_trace_addr:
            do_set_trace_addr(mfd,now,pkg,from,fromlen);
            break;

        case lc_set_rate_limit:
            do_set_rate_limit(mfd,now,pkg,from,fromlen);
            break;

        default:
            break;
    }
}

int tlog_create_mcast_socket(const char* mcast_ip,int gameid, const char *ethx)
{
    int err = 0;
    int mfd = socket(AF_INET,SOCK_DGRAM, 0);
    if(mfd < 0)
    {
        perror("ERROR: Failed to create mcast socket: ");
        return -1;
    }

    tlog_set_socket_nonblock(mfd);
    tlog_set_socket_reuseaddr(mfd);

    uint16_t port;
    if(gameid > 0 && gameid <= MAX_GAMEID)
    {
        port = BASE_SINK_BIND_PORT + gameid;
    }else{
        port = BASE_SINK_BIND_PORT;
    }
    struct sockaddr_in grpaddr;
    grpaddr.sin_family = AF_INET;
    grpaddr.sin_port   = htons(port); 
    inet_pton(AF_INET, mcast_ip, &(grpaddr.sin_addr));

    err = bind(mfd, (const struct sockaddr *)&grpaddr, sizeof(grpaddr));
    if(err < 0)
    {
        fprintf(stderr, "ERROR: Failed to bind mcast addr: %s(%d) - %s\n",mcast_ip,errno,strerror(errno));
        goto ret;
    }

    err = tlog_mcast_join(mfd,(const struct sockaddr *)&grpaddr, sizeof(grpaddr),ethx);
    if(err < 0)
    {
        fprintf(stderr, "ERROR: Failed to join mcast addr: %s(%d) - %s\n",mcast_ip,errno,strerror(errno));
        goto ret;
    }

ret:
    if(err)
    {
        close(mfd);
        mfd = -1;
    }

    return mfd;
}

void *udp_sink_server(void *args)
{
    int err_ret = -1;
    
    char eth0_ip[INET_ADDRSTRLEN];
    int ret = tlog_get_local_eth_ipstr(TLOG_ETH0, eth0_ip);
    if(ret == -1)
    {
        fprintf(stderr, "ERROR: Failed to get eth0's ip\n'");
        pthread_exit(&err_ret);
    }

    const char *inner_eth0_ip_head = "10.1.";
    size_t ipheadlen = strlen(inner_eth0_ip_head);
    const char *ethx = TLOG_ETH1;
    if(strncmp(eth0_ip, inner_eth0_ip_head, ipheadlen) == 0)
        ethx = TLOG_ETH0;

    int mfd = tlog_create_mcast_socket(UDP_SINK_BIND_IP, svc_info->gameid , ethx);
    if(mfd == -1)
        pthread_exit(&err_ret);

    int epoll_ev_nr = 1;
    int epfd = epoll_create(epoll_ev_nr);
    struct epoll_event *evs = calloc(epoll_ev_nr, sizeof(struct epoll_event));
    if(evs == NULL)
        pthread_exit(&err_ret);

    struct epoll_event ev;
    ev.events  = EPOLLIN;
    ev.data.fd = mfd;

epoll_add_again:
    if(tlog_unlikely(epoll_ctl(epfd,EPOLL_CTL_ADD, mfd,&ev) != 0))
    {
        if(errno == EINTR)
            goto epoll_add_again;
        fprintf(stderr, "epoll_ctl add %d error: %m",mfd);
        pthread_exit(&err_ret);
    }

    udp_sink->rate_limit.limit = DEF_RATE_LIMIT;
    while(1)
    {
        int pos,nr;
        nr = epoll_wait(epfd,evs,epoll_ev_nr,500);
        if(tlog_unlikely(nr<0 && errno != EINTR))
            pthread_exit(&err_ret);
        
        struct tm  tm;
        time_t now = time(NULL);
        static time_t last_time = 0;
        if(last_time != now)
        {
            localtime_r(&now,&tm);
            last_time = now;
        }

        for(pos = 0; pos<nr;++pos)
        {
            int curfd = evs[pos].data.fd;
            if(tlog_unlikely(curfd != mfd))
                pthread_exit(&err_ret);

            if(evs[pos].events & EPOLLIN)
            {
                int i;
                struct sockaddr_in from;
                socklen_t fromlen = sizeof(from);
                for(i = 0;i != 5;++i)
                {
                    char *recvbuf     = udp_sink->recvbuf;
                    size_t recvbuflen = sizeof(udp_sink->recvbuf);
                    ssize_t len       = recvfrom(mfd, recvbuf, recvbuflen, 0, (struct sockaddr*)(&from), &fromlen); 
                    if(len > 0)
                    {
                        proc_logger_pkg(mfd, now, recvbuf, len ,&from, fromlen);
                    }else{
                        break;
                    }
                }
            }
        }
    
    }
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

int tlog_get_local_eth_ip(const char *eth, struct in_addr *addr)
{
    int fd;
    struct sockaddr_in *sin;
    struct ifreq ifr;

    if((fd = socket(PF_INET, SOCK_DGRAM ,0)) == -1)
    {
        perror("socket: ");
        return -1;
    }

    // get ip adress
    strncpy(ifr.ifr_name, "eth0", IF_NAMESIZE);
    //ifr.ifr_name[IF_NAMESIZE -1] = '\0';

    if( ioctl(fd, SIOCGIFADDR, &ifr) < 0 )
    {
        perror("ioctl: ");
        close(fd);
        return -1;
    }
    close(fd);

    sin = (struct sockaddr_in *)(&ifr.ifr_addr);
    memcpy(addr, &(sin->sin_addr), sizeof(*addr));

    return 0;
}

int tlog_get_local_eth_ipstr(const char *eth,char *ipbuf)
{
    struct sockaddr_in sin;
    if(tlog_get_local_eth_ip(eth,&(sin.sin_addr)) == -1)
        return -1;
    
    if(inet_ntop(AF_INET, &(sin.sin_addr), ipbuf, INET_ADDRSTRLEN) == NULL)
    {
        perror("inet_ntop: ");
        return -1;
    }

    return 0;
}
