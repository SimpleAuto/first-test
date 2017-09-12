#include "bindconf.h"
#include "config.h"
#include "log.h"

#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

bind_config_t bindconf;

enum{
    bind_conf_max_field_num = 4 
};

/**
 * load_bind_file - parse bind config file @file_name
 * 
 * return: 0 on success, otherwise -1
 */
int load_bind_file(const char* file_name)
{
    int   ret_code = -1;
    char *buf;

    if(mmap_config_file(file_name, &buf) > 0 )
    {
        char *start = buf;
        char *end;
        char *field[bind_conf_max_field_num];
        bind_config_elem_t *bc;

        size_t len = strlen(buf);
        while(buf + len > start)
        {
            end = strchr(start,'\n');
            if( end && *end)
                *end = '\0';

            if( (*start != '#') && (str_split(0,start,field,bind_conf_max_field_num) == bind_conf_max_field_num))
            {
                bc = &(bindconf.configs[bindconf.bind_num]);
                bc->online_id = atoi(field[0]);
                strncpy(bc->online_name, field[1], sizeof(bc->online_name) - 1);
                strncpy(bc->bind_ip    , field[2], sizeof(bc->bind_ip) - 1);
                bc->bind_port = atoi(field[3]);
                ++(bindconf.bind_num);
            }
            start = end + 1;

            if(bindconf.bind_num > max_listen_fds)
            {
                goto exit_entry;
            }
        }

        // munmap用来取消参数start所指的映射内存起始地址，参数length是取消的内存大小
        munmap(buf,len);
        ret_code = 0;
    }
exit_entry:
    BOOT_LOG(ret_code, "load bind file:%s",file_name);
}
