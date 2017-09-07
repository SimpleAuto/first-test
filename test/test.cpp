#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<unistd.h>
#include<math.h>
using namespace std;

#define BOOT_OK   "\e[1m\e[32m[ ok ]\e[m"
#define BOOT_FAIL "\e[1m\e[31m[ failed ]\e[m"

#define __builtin_prefetch(x,y,z) 1

int main()
{
    cout << sin(1) << endl;
    cout << (1<<30) << endl;

    cout << __builtin_prefetch(0,0,0) << endl;
    cout << __builtin_prefetch(3,2,1) << endl;

    char tmp[10] = "hello";
    printf("%s",tmp);
	cout << BOOT_OK << endl;
	cout << BOOT_FAIL << endl;
	printf("%s %s\n",__DATE__,__TIME__);
	char *cur = get_current_dir_name();
	cout<< cur << endl; 
	return 0;
}
