#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "list.h"
#include "hash_r.h"

#define INIKEY_HASHBITS (8)
#define CONFIG_VAL_SIZE (4096)

static int has_init;
static list_head_t ini_key_head[1 << INIKEY_HASHBITS];

static inline int hash_key(const char* key)
{
	return hash_r5(key) & ((1 << INIKEY_HASHBITS)-1);
}

int config_append_value(const char* key,const char *val)
{
	int val_len = strlen(val) +1;
	if(key == NULL || val == NULL || val_len > CONFIG_VAL_SIZE)
		return -1;

	int hash = hash_key(key);
	list_head_t* p = ini_key_head[hash].next;
	while(p != &ini_key_head[hash])
	{
		config_pair_t *mc = list_entry(p,config_pair_t,list);
		if(strlen(mc->key) == strlen(key) && strcmp(mc->key,key)==0)
			return -1;
		p=p->next;
	}

	int len = strlen(key)+1;
	config_pair_t* mc = (config_pair_t*)malloc(sizeof(config_pair_t) +len);
	if(!mc)
		return -1;
	memcpy(mc->key,key,len);
	mc->val = (char*)malloc(CONFIG_VAL_SIZE);
	if(!mc->val)
	{
		free(mc);
		return -1;
	}
	memcpy(mc->val,val,val_len);
	list_add(&mc->list,&ini_key_head[hash]);
	return 0;
}

int config_update_value(const char* key,const char* val)
{
	int val_len = strlen(val) +1;
	if(key == NULL || val == NULL || val_len > CONFIG_VAL_SIZE)
		return -1;

	int hash = hash_key(key);
	list_head_t* p = ini_key_head[hash].next;
	while(p != &ini_key_head[hash])
	{
		config_pair_t *mc = list_entry(p,config_pair_t,list);
		if(strlen(mc->key) == strlen(key) 
			&& strcmp(mc->key,key) ==0
			&& (strlen(mc->val) != strlen(val) || strcmp(mc->val,val) != 0))
		{
			memcpy(mc->val,val,val_len);
			return 0;
		}
		p = p->next;
	}
	return -1;
}

int str_split(const char *ifs,char *line,char *field[],int n)
{
	static const char default_ifs[256]= { [' '] = 1, ['\t'] = 1, ['\r'] = 1, ['\n'] = 1, ['='] = 1 };

	if (ifs == 0) {
		ifs = default_ifs;
	}

	int i = 0;
	while (1) {
		while (ifs[(unsigned char)*line]) {
			++line;
		}
		if (*line == '\0') {
			break;
		}
		field[i++] = line;

		// remove tailing spaces
		if (i >= n) {
			line += strlen(line) - 1;
			while (ifs[(unsigned char)*line]) {
				--line;
			}
			line[1] = '\0';
			break;
		}

		// add a tailing '\0'
		while (*line && !ifs[(unsigned char)*line]) {
			++line;
		}
		if (!*line) {
			break;
		}
		*line++ = '\0';
	}

	return i;

}

static int  config_reset_or_add_value( const char *  k,  const char *  v)
{
	if(config_append_value(k, v) == -1) {
		if(config_update_value(k, v) == -1) {
			return -1;
		}
	}
	return 0;
}

static int parse_config(char *buffer)
{
    static const char myifs[256] = { [' '] = 1,['\t'] = 1,['\r'] = 1,['\n'] = 1,['='] = 1};
	char *filed[2];
	char *start = buffer;
	size_t len = strlen(buffer);
	while(len + buffer > start)
	{
		char *end = strchr(start,'\n');
		if(end)
			*end = '\0';
		if((*start!='#') && (str_split(myifs,start,filed,2) == 2))
		{
			if(config_reset_or_add_value(filed[0],filed[1]) == -1)
				return -1;
		}
		if(end)
		{
			start = end+1;
		}else{
			break;
		}
		
	}
	return 0;
}

int mmap_config_file(const char *file_name,char **buf)
{
	int ret_code = -1;
	int fd = open(file_name,O_RDONLY);
	if(fd == -1)
		return -1;

	int len = lseek(fd,0L,SEEK_END);
	lseek(fd,0L,SEEK_SET);
	
	*buf = (char *)mmap(0,len+1,PROT_READ | PROT_WRITE , MAP_PRIVATE | MAP_ANONYMOUS,-1,0);
	if(*buf != MAP_FAILED)
	{
		read(fd,*buf,len);
		(*buf)[len] = 0;
		ret_code = len +1;
	}
	close(fd);
	return ret_code;
}

int config_init(const char* file_name)
{
	int ret_code = -1;
	if(!has_init)
	{
		for(int i=0;i< (1 << INIKEY_HASHBITS);++i)
			INIT_LIST_HEAD(&ini_key_head[i]);
		has_init = 1;
	}
	
	char *buf;
	int len = mmap_config_file(file_name,&buf);
	if(len > 0)
	{
		ret_code = parse_config(buf);
	}

	return ret_code;
}
