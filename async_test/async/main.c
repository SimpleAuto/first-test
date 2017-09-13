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
	return 0;
}
