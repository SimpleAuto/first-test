#ifndef _TIMER_H_
#define _TIMER_H_

#include <sys/time.h>
#include <time.h>

// export now and tm_cur
extern struct timeval now;
extern struct tm      tm_cur;

/**
 * @brief 更新当前时间。
 * @see get_now_tv, get_now_tm
 */
static inline void renew_now()
{
    gettimeofday(&now,0);
    localtime_r(&now.tv_sec,&tm_cur);
}

/**
 * @brief 对于对实时性要求不会太高的程序，比如摩尔庄园的Online Server这种循环处理客户端发过来的数据包的程序，
 *        可以先调用一次renew_now，然后处理N个数据包，然后再调用一次renew_now，接着再处理N个数据包，如此反复。
 *        这样在处理数据包的函数里就可以直接使用get_now_tv来获取不太精确的当前时间，从而能稍微提升程序的效率。
 * @return 不太精确的当前时间。
 * @see renew_now, get_now_tm
 */
static inline const struct timeval* get_now_tv()
{
	return &now;
}


#endif // _TIMER_H_
