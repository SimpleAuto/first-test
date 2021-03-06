/*
 *************************************************************************
 *    File Name:  service.c
 *       Author: Thomas Shanghai,Inc.
 *         Mail: 982737593@qq.com
 * Created Time: 2017年09月22日 星期五 15时38分38秒
 * 
 *  Description: ...
 * 
 ************************************************************************
*/
#include <stdio.h>
#include <glib.h>

#include "bindconf.h"
#include "config.h"
#include "tlog.h"
#include "tlog_mcast.h"
#include "tlog_decl.h"
#include "util.h"
#include "shmq.h"
#include "net.h"
#include "daemon.h"
#include "dll.h"
#include "mcast.h"
#include "service.h"

int                is_parent = 1;
config_cache_t     config_cache;
fd_array_session_t fds;

static inline void free_fdsess(void *fdsess)
{
    g_slice_free1(sizeof(fdsession_t), fdsess);
}

static inline int handle_init(bind_config_elem_t* bc_elem)
{
    config_cache.idle_timeout = config_get_intval("idle_timeout", 10);
    config_cache.bc_elem      = bc_elem;
    fds.cn                    = g_hash_table_new_full(g_int_hash, g_int_equal, 0, free_fdsess);

    if(dll.proc_mcast_pkg && (asynsvr_create_mcast_socket() == -1))
        return -1;

    if(config_get_strval("addr_mcast_ip"))
    {
        if(create_addr_mcast_socket() == 0)
            send_addr_mcast_pkg(addr_mcast_1st_pkg);
        else
            return -1;
    }

    return (dll.init_service ? dll.init_service(0) :0);
}

void run_worker_process(bind_config_t* bc, int bc_elem_idx, int n_inited_bc)
{
    bind_config_elem_t * bc_elem = &(bc->configs[bc_elem_idx]);

    char prefix[10] = { 0 };
    int  len        = snprintf(prefix, 8, "%u", bc_elem->online_id);
    prefix[len]     = '_';

#ifdef USE_TLOG
    char prefix_1[10] = { 0 };
    snprintf(prefix_1, 10, "%d", bc_elem->online_id);
    INIT_DEFAULT_LOGGER_SYSTEM( config_get_strval("log_dir"),
                                prefix_1,
                                config_get_strval("project_name"),
                                config_get_intval("project_id",0),
                                config_get_strval("svc_type"));

    int log_time_interval = config_get_intval("tlog_file_interval_sec", 900);
    if(log_time_interval < 0 || log_time_interval > 86400)
        log_time_interval = 900;
    
    SET_MAX_ONE_SIZE(100);
    SET_TIME_SLICE_SECS(log_time_interval);
    SET_LOG_LEVEL(config_get_intval("log_level", tlog_lvl_debug));
#endif

    log_init_ex( config_get_strval("log_dir"),
                 config_get_intval("log_level", tlog_lvl_trace),
                 config_get_intval("log_size",  1<<30),
                 config_get_intval("max_log_files", 100),
                 prefix,
                 config_get_intval("log_save_next_file_interval_min", 0)
                 );

    is_parent = 0;

    close_shmq_pipe(bc , n_inited_bc, 1);
    shmq_destory(bc_elem, n_inited_bc);
    net_exit();

    daemon_set_title("%s-%u", prog_name, bc_elem->online_id);

    net_init(max_fd_num, 2000);
    do_add_conn(bc_elem->recvq.pipe_handles[0], fd_type_pipe, 0 ,0);

    if( handle_init(bc_elem) != 0 )
    {

    }
}
