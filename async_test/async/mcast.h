/*
 *************************************************************************
 *    File Name:  mcast.h
 *       Author: Thomas Shanghai,Inc.
 *         Mail: 982737593@qq.com
 * Created Time: 2017年09月22日 星期五 15时38分16秒
 * 
 *  Description: ...
 * 
 ************************************************************************
*/
#ifndef _MCAST_H_
#define _MCAST_H_

// inet/mcast.h

typedef enum mcast_flag
{
    /*! 创建的组播socket只用于读取组播包 */
    mcast_rdonly = 1,
    /*! 创建的组播socket只用于发送组播包 */
    mcast_wronly = 2,
    /*! 创建的组播socket既能读取组播包，也能发送组播包 */
    mcast_rdwr   = mcast_rdonly | mcast_wronly
}mcast_flag_t;

/**
  * @brief 加入一个多播组。
  *
  * @param sockfd  加入到多播组的sockfd。成功加入多播组后，应用程序可以通过这个sockfd来获取来自grp的多播包。
  * @param grp     想要加入的多播地址。
  * @param grplen  grp的大小（byte）。
  * @param ifname  加入到多播组的网络接口，比如eth1。成功加入多播组后，内核将使用这个接口来接收来自grp的多播包。
  *
  * @see mcast_leave
  *
  * @return int, 0 成功, -1 失败。
  */
int mcast_join(int sockfd, const struct sockaddr* grp, socklen_t grplen,const char * ifname);

/**
  * @brief 设置组播包的网络出口。
  *
  * @param sockfd  用来发多播包的sockfd。
  * @param family  sockfd的address family。
  * @param ifname  多播包的网络出口，比如eth1。
  *
  * @return int, 0 成功, -1 失败。
  */
int mcast_set_if(int sockfd, int family, const char* ifname);

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
int create_mcast_socket(const char* mcast_addr, const char* port,const char* nif,mcast_flag_t flag, struct sockaddr_storage *ss, socklen_t* addrlen);

// mcast.h
 
enum 
{
	mcast_notify_addr   = 0,
	mcast_reload_text	= 1
};

enum
{
	addr_mcast_1st_pkg	= 1,
	addr_mcast_syn_pkg	= 2
};

struct addr_node 
{
	uint32_t		svr_id;
	char			ip[16];
	unsigned short	port;
	time_t			last_syn_tm;
};

typedef struct addr_node addr_node_t;
extern time_t next_syn_addr_tm;
extern time_t next_del_addrs_tm;

int asynsvr_create_mcast_socket();
int  create_addr_mcast_socket();
void send_addr_mcast_pkg(uint32_t pkg_type);

#endif // _MCAST_H_
