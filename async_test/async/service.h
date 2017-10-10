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
#include "bindconf.h"

extern int is_parent;

typedef struct fdsession 
{
	int			fd;
	uint32_t	id;
	uint16_t	remote_port;
	uint32_t	remote_ip;
} fdsession_t;

void run_worker_process(bind_config_t* bc, int bc_elem_idx, int n_inited_bc);

#endif // _SERVICE_H_
