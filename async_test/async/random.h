/*
 *************************************************************************
 *    File Name:  random.h
 *       Author: Thomas Shanghai,Inc.
 *         Mail: 982737593@qq.com
 * Created Time: 2017年10月11日 星期三 18时05分14秒
 * 
 *  Description: ...
 * 
 ************************************************************************
*/
#ifndef _RANDOM_H_
#define _RANDOM_H_

/**
 * @brief 产生给定范围内的随机数。如果需要更加随机，可以在程序起来的时候调用一次srand()。
 * @param min 随机出来的最小数。
 * @param max 随机出来的最大数。
 * @return min和max之间的随机数，包括min和max。
 */
static inline int ranged_random(int min, int max)
{
    // generates ranged random number
    return (rand() % (max - min + 1)) + min;
}

#endif // _RANDOM_H_
