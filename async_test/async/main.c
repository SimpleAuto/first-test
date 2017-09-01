#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include "log.h"
#include "config.h"
#include "list.h"
#include "deamon.h"

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

  //  printf("0\n");	
  //  char tmp[10] = "hello";
  //  BOOT_LOG(0,"wawawa,%s",tmp);
  //  printf("1\n");	

	parse_args(argc,argv);
	char *p_conf_file=argv[1];
	//int use_lua_config=0;
	
	if(config_init(p_conf_file) == -1){
		BOOT_LOG(-1, "Failed To Parse config file '%s'",argv[1]);
	}

    daemon_start(argc, argv);

	return 0;
}
