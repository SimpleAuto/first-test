#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include "log.h"
#include "config.h"
#include "list.h"
#include "daemon.h"
#include "timer.h"
#include "bindconf.h"
#include "util.h"
#include "net.h"
#include "dll.h"

char g_progame_name[256];
char *prog_name;
char *current_dir;
static const char version[] = "1.6.4.1";

static inline void show_banner()
{
	char feature[256];
	int pos = 0;

#ifdef DEBUG
	pos = sprintf (feature + pos,"-DDEBUG -g ");
#endif
	pos = sprintf (feature + pos,"-DEPOLL_MODE ");

	printf("Async Server v%s (C)",version);
	printf("Compiled at %s %s,flag: %s\n",__DATE__,__TIME__,pos ? feature : "");
}

static inline void show_usage()
{
	exit(-1);
}

static inline void parse_args(int argc,char **argv)
{
	prog_name   = strdup(argv[0]);
	current_dir = get_current_dir_name();
	show_banner();
	if( (argc<2) || !strcmp(argv[1],"--help") || !strcmp(argv[1],"-h")){
		show_usage();
	}
}

int main(int argc,char *argv[])
{
	// 将程序名称放在 g_progame_name变量中存储
	strncpy(g_progame_name,argv[0],sizeof(g_progame_name)-1 );
	g_progame_name[sizeof(g_progame_name)-1] = '\0';

	parse_args(argc,argv);
	char *p_conf_file=argv[1];
	//int use_lua_config=0;
	
	if(config_init(p_conf_file) == -1){
		BOOT_LOG(-1, "Failed To Parse config file '%s'",argv[1]);
	}

    daemon_start(argc, argv);
    // 更新当前时间
    renew_now();

    load_bind_file(config_get_strval("bind_conf"));

#ifdef USE_TLOG
    INIT_DEFAULT_LOGGER_SYSTEM( config_get_strval("log_dir"),
                                "0",
                                config_get_strval("project_name"),
                                config_get_intval("project_id",0),
                                config_get_strval("svc_type"));
    int log_time_interval = config_get_intval("tlog_file_interval_sec",900);
    if(log_time_interval < 0 || log_time_interval > 86400)
        log_time_interval = 900;
    SET_MAX_ONE_SIZE(100);
    SET_TIME_SLICE_SECS(log_time_interval);
    SET_LOG_LEVEL(config_get_intval("log_level", tlog_lvl_debug));
#endif

    // 初始化log目录设置，以及log等级
    log_init_ex( config_get_strval("log_dir"),
                 config_get_intval("log_level",    log_lvl_trace),
                 config_get_intval("log_size" ,    1<<30),
                 config_get_intval("max_log_size", 100),
                 config_get_strval("log_prefix"),
                 config_get_intval("log_save_next_file_interval_min", 0));

    socket_timeout       = config_get_intval("cli_socket_timeout"      ,  0);
    page_size             = config_get_intval("incoming_packet_max_size", -1);
    g_send_buf_limit_size = config_get_intval("send_buf_limit_size"     ,  0);
    if(page_size <= 0)
        page_size = def_page_size_;

    KDEBUG_LOG(0,"123456");
    KDEBUG_LOG(0,"1234567");

    // 注册动态加载相关
    // 这个需要之后实现
    register_data_plugin(config_get_strval("data_dll_file"));
    register_plugin(config_get_strval("dll_file"), 0);

    net_init(max_fd_num, max_fd_num);
    if(dll.init_service && (dll.init_service(1) != 0))
    {
        BOOT_LOG(-1, "FAILED TO INIT PARENT PROCESS");
    }

    clean_child_pids();

    bind_config_t* bc = get_bind_conf();

	return 0;
}
