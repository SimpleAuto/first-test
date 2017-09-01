#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "list.h"

typedef struct config_pair{
	struct list_head list;
	char *val;
	char key[];
}config_pair_t;

int config_init(const char* file_name);
int mmap_config_flie(const char* file_name,char **buf);
//int parse_config(char **buf);
int str_split(const char *ifs,char *line,char *field[],int n); 
int config_append_value(const char* key,const char *val);
int config_update_value(const char* key,const char *val);


#endif //_CONFIG_H_
