#include "dll.h"
#include "log.h"
#include "types.h"

#include <dlfcn.h>

async_serv_if_t dll;

#define DLFUNC_NO_ERROR(h, v, name) \
		do { \
			v = dlsym(h, name); \
			dlerror(); \
		} while (0)

#define DLFUNC(h, v, name) \
		do { \
			v = dlsym (h, name); \
			if ((error = dlerror ()) != NULL) { \
				ERROR_LOG("dlsym error, %s", error); \
				dlclose(h); \
				h = NULL; \
				goto out; \
			} \
		} while (0)

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

int register_plugin(const char* file_name, int flag)
{
    char * error;
    int   ret_code = -1;

    dll.handle = dlopen(file_name, RTLD_NOW);
    if((error = dlerror()) != NULL)
    {
        ERROR_LOG("dlopen error, %s", error);
        goto out;
    }

    // 实际调用dlsym 动态加载函数地址
    DLFUNC_NO_ERROR(dll.handle, dll.init_service, "init_service");
	DLFUNC_NO_ERROR(dll.handle, dll.fini_service, "fini_service");
	DLFUNC_NO_ERROR(dll.handle, dll.proc_events, "proc_events");
	DLFUNC_NO_ERROR(dll.handle, dll.proc_mcast_pkg, "proc_mcast_pkg");
	DLFUNC_NO_ERROR(dll.handle, dll.proc_udp_pkg, "proc_udp_pkg");

	DLFUNC(dll.handle, dll.get_pkg_len, "get_pkg_len");
	DLFUNC(dll.handle, dll.proc_pkg_from_client, "proc_pkg_from_client");
	DLFUNC(dll.handle, dll.proc_pkg_from_serv, "proc_pkg_from_serv");
	DLFUNC(dll.handle, dll.on_client_conn_closed, "on_client_conn_closed");
	DLFUNC(dll.handle, dll.on_fd_closed, "on_fd_closed");

	DLFUNC_NO_ERROR(dll.handle, dll.before_reload, "before_reload");
	DLFUNC_NO_ERROR(dll.handle, dll.reload_global_data, "reload_global_data");
	DLFUNC_NO_ERROR(dll.handle, dll.sync_service_info, "sync_service_info");

    ret_code = 0;

out:
    if(!flag)
    {
        BOOT_LOG(ret_code, "dlopen %s", file_name);
    } else{
        DEBUG_LOG("RELOAD %s\t[%s]", file_name, (ret_code ? "FAIL" : "OK"));
        return ret_code;
    }

}
