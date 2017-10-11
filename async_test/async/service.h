/*
 *************************************************************************
 *    File Name:  service.h
 *       Author: Thomas Shanghai,Inc.
 *         Mail: 982737593@qq.com
 * Created Time: 2017年09月16日 星期六 19时41分39秒
 * 
 *  Description: ...
 * 
 ************************************************************************
*/
#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <stdint.h>
#include <glib.h>
#include "bindconf.h"


typedef struct 
{
    int                 idle_timeout;
    bind_config_elem_t* bc_elem;
}config_cache_t;

typedef struct fd_array_session
{
    int         count;
    GHashTable *cn;
}fd_array_session_t;

typedef struct fdsession 
{
	int			fd;
	uint32_t	id;
	uint16_t	remote_port;
	uint32_t	remote_ip;
} fdsession_t;

extern int                is_parent;
extern config_cache_t     config_cache;
extern fd_array_session_t fds;

void run_worker_process(bind_config_t* bc, int bc_elem_idx, int n_inited_bc);

#endif // _SERVICE_H_
