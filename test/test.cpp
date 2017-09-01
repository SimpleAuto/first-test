#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<unistd.h>
using namespace std;

#define BOOT_OK   "\e[1m\e[32m[ ok ]\e[m"
#define BOOT_FAIL "\e[1m\e[31m[ failed ]\e[m"

int main()
{
    char tmp[10] = "hello";
    printf("%s",tmp);
	cout << BOOT_OK << endl;
	cout << BOOT_FAIL << endl;
	printf("%s %s\n",__DATE__,__TIME__);
	char *cur = get_current_dir_name();
	cout<< cur << endl; 
	return 0;
}
