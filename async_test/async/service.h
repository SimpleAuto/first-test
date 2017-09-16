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

typedef struct fdsession 
{
	int			fd;
	uint32_t	id;
	uint16_t	remote_port;
	uint32_t	remote_ip;
} fdsession_t;

#endif // _SERVICE_H_
