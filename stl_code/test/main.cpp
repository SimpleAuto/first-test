/*
 *************************************************************************
 *    File Name:  test.cpp
 *       Author: Thomas Shanghai,Inc.
 *         Mail: 982737593@qq.com
 * Created Time: 2017年10月12日 星期四 15时01分42秒
 * 
 *  Description:     这个文件是用来在阅读stl这本书时，按照书上的内容进行针
 *               对型练习，可以熟练使用这些容器以及function，方便以后的开
 *               发工作。
 * 
 *************************************************************************
*/
#include "main.h"

#include <iostream>
using namespace std;

#if _HASHTABLE
#include <hashtable.h>
#include <hash_fun.h>
#include <bits/stl_function.h>
#include <ext/functional>
using namespace __gnu_cxx;
int main()
{
    // 28个质数
    // 53,97,193,389,769,1543,3079,6151...........
    // hashtable < value, key, hash_func, extract-key, equal-key,alloc(default)
    // identity<T> 一元函数，代表了一个恒等函数，接受一个参数x，返回参数x
    hashtable<int ,int , hash<int>, identity<int>, equal_to<int> > iht(50, hash<int>(), equal_to<int>());
    // size() 表示 元素个数
    cout << iht.size()         << endl;
    // bucket_count() 表示 质数(容器大小最接近的质数)
    cout << iht.bucket_count() << endl;
    // max_bucket_count() 表示 容器最大可容纳的元素个数
    cout << iht.max_bucket_count() << endl;

    // value重复插入失败
    iht.insert_unique(59);
    iht.insert_unique(63);
    iht.insert_unique(108);
    iht.insert_unique(2);
    iht.insert_unique(53);
    iht.insert_unique(55);
    iht.insert_unique(63);
    
    // 迭代器输出
    hashtable<int ,int , hash<int>, identity<int>, equal_to<int> >::iterator hash_it = iht.begin();
    for(; hash_it != iht.end(); ++hash_it)
    {
        cout << *hash_it << " ";
    }
    cout << endl;

    // 遍历所有buckets, 如果其节点个数不为0, 就打印出节点个数
    for(int i =0; i<iht.bucket_count();++i)
    {
        int num = iht.elems_in_bucket(i);
        if(num != 0)
            cout << "bucket[" << i << "] has " << num << " elems" << endl;
    }
    return 0;
}
#endif
