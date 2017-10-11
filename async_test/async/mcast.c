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
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <glib.h>

#include "config.h"
#include "log.h"
#include "net.h"
#include "inet_utils.h"
#include "timer.h"
#include "service.h"
#include "random.h"
#include "dll.h"
#include "mcast.h"

#pragma pack(1)
struct mcast_pkg_header 
{
	uint16_t	pkg_type;   // for mcast_notify_addr: 1st, syn
	uint16_t	proto_type; // mcast_notify_addr, mcast_reload_text
	char		body[];
};
struct addr_mcast_pkg 
{
	uint32_t	svr_id;
	char		name[16];
	char		ip[16];
	uint16_t	port;
};
struct reload_text_pkg 
{
	char		svr_name[16];
	uint32_t	svr_id;
	char		new_so_name[32];
};
#pragma pack()
struct addr_cache 
{
	char		svr_name[16];
	GHashTable*	addr_tbl;
};

typedef struct mcast_pkg_header mcast_pkg_header_t;
typedef struct addr_mcast_pkg addr_mcast_pkg_t;
typedef struct reload_text_pkg reload_text_pkg_t;
typedef struct addr_cache addr_cache_t;

time_t next_syn_addr_tm  = 0x7FFFFFFF;
time_t next_del_addrs_tm = 0x7FFFFFFF;

static GHashTable* svr_tbl;
static char addr_buf[sizeof(mcast_pkg_header_t) + sizeof(addr_mcast_pkg_t)];
// multicast socket for service name synchronization
static int addr_mcast_fd = -1;
static struct sockaddr_storage addr_mcast_addr;
static socklen_t addr_mcast_len;
// multicast socket
static int       mcast_fd = -1;
static struct    sockaddr_storage mcast_addr;
static socklen_t mcast_len;

// translate the given address family to its corresponding level
static inline int family_to_level(int family)
{
	int level = -1;

	switch (family) {
	case AF_INET:
		level = IPPROTO_IP;
		break;
	case AF_INET6:
		level = IPPROTO_IPV6;
		break;
	default:
		break;
	}

	return level;
}

static void free_addr_cache(void* addr_cache)
{
	addr_cache_t* addr_c = addr_cache;
	g_hash_table_destroy(addr_c->addr_tbl);
	g_slice_free1(sizeof(*addr_c), addr_c);
}
// ========================================================

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
    
    struct addrinfo  hints;
    struct addrinfo *res  = 0;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags    = 0;
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    int err = getaddrinfo(mcast_addr, port, &hints, &res);
    if(err != 0)
    {
        errno = eai_to_errno(err);
        return -1;
    }

    int mcastfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(mcastfd < 0)
    {
        err = errno;
        freeaddrinfo(res);
        errno = err;
        return -1;
    }

    if(flag & mcast_rdonly)
    {
        int reuse_addr = 1;
        err = setsockopt(mcastfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
        if(err < 0)
            goto ret;

        err = bind(mcastfd, res->ai_addr, res->ai_addrlen);
        if(err < 0)
            goto ret;

        err = mcast_join(mcastfd, res->ai_addr, res->ai_addrlen, nif);
        if(err < 0)
            goto ret;
    }

    if(flag & mcast_wronly)
    {
        err = mcast_set_if(mcastfd, res->ai_family, nif);
        if(err < 0)
            goto ret;
    }

    if(ss)
    {
        memcpy(ss, res->ai_addr, res->ai_addrlen);
        *addrlen = res->ai_addrlen;
    }

ret:
    if(err)
    {
        err = errno;
        close(mcastfd);
        mcastfd = -1;
    }
    freeaddrinfo(res);
    errno = err;
    return mcastfd;
}

int mcast_join(int sockfd, const struct sockaddr* grp, socklen_t grplen,const char * ifname)
{
	struct group_req req;

	assert(grplen <= sizeof(req.gr_group));

	req.gr_interface = if_nametoindex(ifname);
	if (req.gr_interface == 0) {
		errno = ENXIO; /* i/f name not found */
		return -1;
	}

	memcpy(&req.gr_group, grp, grplen);
	return setsockopt(sockfd, family_to_level(grp->sa_family), MCAST_JOIN_GROUP, &req, sizeof(req));
}

int mcast_set_if(int sockfd, int family, const char* ifname)
{
	switch (family) {
	case AF_INET:
	{
		struct ifreq ifreq;

		strncpy(ifreq.ifr_name, ifname, IFNAMSIZ);
		if (ioctl(sockfd, SIOCGIFADDR, &ifreq) < 0) {
			return -1;
		}

		struct in_addr*	inaddr = &((struct sockaddr_in*)&ifreq.ifr_addr)->sin_addr;
		return setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, inaddr, sizeof(*inaddr));
	}
	case AF_INET6:
	{
		u_int idx = if_nametoindex(ifname);
		if (idx == 0) {
			errno = ENXIO; /* i/f name not found */
			return -1;
		}

		return setsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_IF, &idx, sizeof(idx));
	}
	default:
		errno = EAFNOSUPPORT;
		return -1;
	}
}

// =========================================================
int create_addr_mcast_socket()
{
	srand(time(0));

	next_del_addrs_tm = get_now_tv()->tv_sec + 100;
	svr_tbl = g_hash_table_new_full(g_str_hash, g_str_equal, 0, free_addr_cache);

	addr_mcast_fd = create_mcast_socket(config_get_strval("addr_mcast_ip"),
										config_get_strval("addr_mcast_port"),
										config_get_strval("addr_mcast_incoming_if"),
										mcast_rdwr, &addr_mcast_addr, &addr_mcast_len);
	if (addr_mcast_fd == -1) {
		ERROR_LOG("Failed to Create `addr_mcast_fd`: err=%d %s", errno, strerror(errno));
		return -1;
	}

	if (!is_parent) {
		mcast_pkg_header_t* hdr = (void*)addr_buf;
		addr_mcast_pkg_t*   pkg = (void*)hdr->body;
	
		hdr->proto_type = mcast_notify_addr;
		pkg->svr_id     = get_server_id();
		strcpy(pkg->name, get_server_name());
		strcpy(pkg->ip, get_server_ip());
		pkg->port       = get_server_port();
	}

	return do_add_conn(addr_mcast_fd, fd_type_addr_mcast, (void*)&addr_mcast_addr, 0);
}

void send_addr_mcast_pkg(uint32_t pkg_type)
{
	mcast_pkg_header_t* hdr = (void*)addr_buf;

	hdr->pkg_type = pkg_type;
	sendto(addr_mcast_fd, addr_buf, sizeof(addr_buf), 0, (void*)&addr_mcast_addr, addr_mcast_len);
	next_syn_addr_tm  = time(0) + ranged_random(25, 48);
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
