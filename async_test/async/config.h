#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "list.h"
#include "global.h"

typedef struct config_pair{
	struct list_head list;
	char *val;
	char key[];
}config_pair_t;

/**
  * @brief 解析配置文件file_name。成功返回后就可以调用config_get_inval或者config_get_strval
  *         来获取配置文件的内容了。
  * @param file_name 配置文件路径名。
  * @see config_get_intval, config_get_strval
  * @return 成功则返回0，失败则返回-1。
  */
int config_init(const char* file_name);

// return -1 on error
int mmap_config_file(const char* file_name,char **buf);

// return number of words splitted
int str_split(const char *ifs,char *line,char *field[],int n); 

/**
 * @brief add a new item to configuration
 * @param key which item you want to add
 * @param val corresponded val with key
 * @return -1 key or val is NULL || conflict key || malloc failed
 * @return  0 on success
 *
 */
int config_append_value(const char* key,const char *val);

/** 
 * @brief update a config item based on key and val
 * @param key which item you want to update
 * @param val coressponded new val
 * @return -1 key or val is NULL || no key exist || or update failed
 * @return 0  on success
 */
int config_update_value(const char* key,const char *val);

/**
  * @brief 获取配置项key的整型值。如果找不到key，则返回值为def。如果key对应的值是非数字字符，则行为未定义。
  *        故请小心校验配置文件和代码。
  * @param key 配置项key。
  * @param def 如果配置文件中没有配置项key，则返回def。
  * @see config_get_strval
  * @return 配置项key对应的整型值，或者def。
  */
int config_get_intval(const char*key,int def);

/**
  * @brief 获取配置项key的字符创型的值。
  * @param key 配置项key。
  * @see config_get_intval
  * @return 配置项key对应的字符串型的型值。这个指针在调用config_exit之前一直都是有效可用的。
  */
char* config_get_strval(const char* key);


#endif //_CONFIG_H_
