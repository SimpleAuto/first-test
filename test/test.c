#define NOTUSE (0)
#define USEING (1)

/*
 * TODO: I/O复用之select示例 
 * 
 */
#if USEING

#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stlib.h>
#include <errno.h>
#include <fcntl.h>

int main()
{
    // give ip and port
    const char *ip = "192.168.72.131";
    int port = 8080;
    
    // set ip and port and family to sockaddr_in
    struct sockaddr_in address;
    bzero(&address,sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET,ip,&address.sin_addr);
    address.sin_port = htons(port);

    int ret = 0;
    int listenfd = socket(PF_INET,SOCK_STREAM,0);
    assert( listenfd >= 0);
    ret = bind( listenfd , ( struct sockaddr* )&address , sizeof( address ));
    assert(ret != -1);
    ret = listen( listenfd , 5 );
    assert(ret != -1);

    return 0;
}
#endif

/*
 * TODO:验证math.h是否可用 
 */
#if NOTUSE

#include<math.h>
#include<stdio.h>

int main()
{
    int i = (int)sin(10);
    printf("%d\n",i);
    return 0;
}
#endif
