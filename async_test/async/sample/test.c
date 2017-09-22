#include "../service.h"
#include <libtaomee/log.h>

#include "test.h"

int test(void* owner, void* data)
{
	return 0;
}

int init_service(int isparent)
{
	if (!isparent) {
	}
	return 0;
}

int fini_service(int isparent)
{
	return 0;
}

void proc_events()
{
}

int	get_pkg_len(int fd, const void* pkg, int pkglen, int isparent)
{
	if (isparent) {
	}else{
    }

	return 0;
}

int proc_pkg_from_client(void* data, int len, fdsession_t* fdsess)
{
	return 0;
}

void proc_pkg_from_serv(int fd, void* data, int len)
{
}

void on_client_conn_closed(int fd)
{
}

void on_fd_closed(int fd)
{
}

