#include "tlog.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

void boot_tlog(int ok,int dummy,const char* fmt,...)
{
#define SCREEN_COLS 80
#define BOOT_OK   "\e[1m\e[32m[ ok ]\e[m"
#define BOOT_FAIL "\e[1m\e[31m[ failed ]\e[m"
// BOOT_OK [ ok ]  BOOT_FAIL [ failed ]
	int end,i,pos;
	va_list ap;
	
	char buffer[TLOG_BUF_SIZE];
	va_start(ap,fmt);
	end = vsnprintf(buffer, sizeof(buffer)-1 ,fmt ,ap);
	va_end(ap);

	pos = SCREEN_COLS - 10 - (end-dummy) % SCREEN_COLS;
	for(i=0;i<pos;++i)
		buffer[end+i] = ' ';

	buffer[end+i] = '\0';
	
	strcat(buffer,ok==0? BOOT_OK : BOOT_FAIL);
	printf("%s\n",buffer);
	if(ok)
		exit(ok);
}
