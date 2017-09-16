#include "dll.h"
#include "log.h"
#include "types.h"

#include <dlfcn.h>

async_serv_if_t dll;

int register_data_plugin(const char* file_name)
{
    char *error;
    int   ret_code = 0;
    if(file_name == NULL)
        return 0;
    
    // 打开一个动态链接库，并返回动态链接库的句柄
    dll.data_handle = dlopen(file_name, RTLD_NOW | RTLD_GLOBAL);
    if((error = dlerror()) != NULL)
    {
        ERROR_LOG("dlopen error, %s", error);
        ret_code = 0;
    }
    BOOT_LOG(ret_code, "dlopen %s", file_name);
}
