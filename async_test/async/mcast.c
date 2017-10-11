/*
 *************************************************************************
 *    File Name:  mcast.c
 *       Author: Thomas Shanghai,Inc.
 *         Mail: 982737593@qq.com
 * Created Time: 2017年09月22日 星期五 15时38分21秒
 * 
 *  Description: ...
 * 
 ************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>

#include "config.h"
#include "log.h"
#include "net.h"
#include "mcast.h"

// multicast socket
static int       mcast_fd = -1;
static struct    sockaddr_storage mcast_addr;
static socklen_t mcast_len;

/**
 * @brief 创建一个组播socket。目前只支持IPv4。
 * @param mcast_addr 组播IP地址。只能使用239.X.X.X网段的组播地址。
 * @param port 组播端口。
 * @param nif 网卡接口（eth0/eth1/lo...）。用于指定通过哪个网卡接口收发组播包。
 * @param flag 指定组播socket的用途。mcast_rdonly表示创建的组播socket只用于读取组播包；
 *             mcast_wronly表示创建的组播socket只用于发送组播包；
 *             mcast_rdwr表示创建的组播socket既能读取组播包，也能发送组播包。
 * @param ss 返回初始化好的sockaddr_in/sockaddr_in6结构体，用于sendto发送组播包。
 * @param addrlen 返回ss的有效大小（字节），供sendto使用。
 * @return 成功返回创建好的组播socket fd，失败返回-1。
 */
int create_mcast_socket(const char* mcast_addr, const char* port,const char* nif,mcast_flag_t flag, struct sockaddr_storage *ss, socklen_t* addrlen)
{
    // forbid any IP address other than those begin with 239
    // use 239 because 239仅在特定的本地范围内有效，
    // 239.0.0.0~239.255.255.255 为本地管理组播地址
    if(strncmp(mcast_addr, "239",3) != 0)
    {
        errno = EADDRNOTAVAIL;
        return -1;
    }
    return 0;
}

int asynsvr_create_mcast_socket()
{
    mcast_fd = create_mcast_socket(config_get_strval("mcast_ip"),
                                   config_get_strval("mcast_port"),
                                   config_get_strval("mcast_incoming_if"),
                                   mcast_rdwr, &mcast_addr, &mcast_len);
    if(mcast_fd == -1)
    {
        ERROR_LOG("Failed to Create 'mcast_fd': err=%d %s",errno, strerror(errno));
        return -1;
    }

    return do_add_conn(mcast_fd, fd_type_mcast, (void*)&mcast_addr, 0);
}
