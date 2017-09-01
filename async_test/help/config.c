/*
* 针对于 config.c/h文件，对其中的函数加以说明
*/
// 计算hash值
static inline int hash_key(const char* key);
//  将配置文件的内容经过hash之后，添加到list中
int config_append_value(const char* key,const char *val);
// 将配置文件的内容如果存在，即更新对应list的key中的value
int config_update_value(const char* key,const char* val);
//字符串切割函数 将字符串的尾部更改为'\0'结尾
int str_split(const char *ifs,char *line,char *field[],int n);
//把文件内容读到buff中保存
int mmap_config_file(const char *file_name,char **buf);
// 解析配置文件 调用的入口函数
static int parse_config(char *buffer);
// 配置文件初始化，先初始化256个头节点，然后调用parse_config将文件数据添加到hash之后对应的头节点的链表中
int config_init(const char* file_name);
