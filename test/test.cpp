#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<unistd.h>
#include<math.h>
using namespace std;

#define BOOT_OK   "\e[1m\e[32m[ ok ]\e[m"
#define BOOT_FAIL "\e[1m\e[31m[ failed ]\e[m"

#define __builtin_prefetch(x,y,z) 1
#define tlog_unlikely(x) __builtin_expect(!!(x),0)

int main()
{
    cout << "-==================-" << endl;
    cout << !1                     << endl;
    cout << !0                     << endl;
    cout << !2                     << endl;
    cout << !!(1)                  << endl;
    cout << !!(0)                  << endl;
    cout << !!(2)                  << endl;
    cout << "-==================-" << endl;

    cout << "+==================+" << endl;
    cout << tlog_unlikely(1)       << endl;
    cout << tlog_unlikely(2 == 2)  << endl;
    cout << tlog_unlikely(3 == 2)  << endl;
    cout << tlog_unlikely(3 == 3 || 3)  << endl;
    cout << tlog_unlikely(3 == 2 || 1)  << endl;
    cout << tlog_unlikely(3 == 2 || 0)  << endl;
    cout << "+==================+" << endl;

    cout << "test for math.h->sin:                     " << sin(1) << endl;
    cout << "test for bit operation:                   " << (1<<30) << endl;
    cout << "test for define __builtin_prefetch(x,x,x) " << __builtin_prefetch(0,0,0) << endl;
    cout << "test for define __builtin_prefetch(x,y,z) " << __builtin_prefetch(3,2,1) << endl;

	cout << "test for define BOOT_OK:                  " << BOOT_OK << endl;
	cout << "test for define BOOT_FAIL:                " << BOOT_FAIL << endl;
    cout << "test for define __DATE__:                 " << __DATE__ << endl;
    cout << "test for define __TIME__:                 " << __TIME__ << endl;
	char *cur = get_current_dir_name();
	cout<< "cout current dir name:                     " << cur << endl; 
	return 0;
}
