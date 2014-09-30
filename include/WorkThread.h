/*************************************************************************
	> File Name: WorkThread.h
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年09月29日 星期一 23时06分42秒
 ************************************************************************/
#ifndef _MAXWELL_WORKTHREAD_H_
#define _MAXWELL_WORKTHREAD_H_

#include "Thread.h"

class ThreadPool;

class WorkThread:public Thread{
public:
    void regeditPool(ThreadPool* p_pool);
    void workking();
private:
    ThreadPool* _p_pool; 
};

#endif
