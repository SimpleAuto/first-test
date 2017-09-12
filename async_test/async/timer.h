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

#endif // _TIMER_H_
