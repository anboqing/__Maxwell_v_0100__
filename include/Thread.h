/*************************************************************************
	> File Name: Thread.h
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年09月29日 星期一 22时09分29秒
 ************************************************************************/
#ifndef _MAXWELL_THREAD_H_
#define _MAXWELL_THREAD_H_

#include <pthread.h>
#include "Uncopyable.h"

class Thread:public Uncopyable{
public:
    //构造函数和析构函数
    Thread();
    virtual ~Thread();

    //这是线程的处理函数
    static void *thread_func(void*) ;

    // 这个是线程启动函数
    void start();
    // 这是线程的结束函数 别的线程调用此函数 回收本线程的资源
    void exit();

    void join(pthread_t&);
    //这个是线程类要执行的具体任务函数
    virtual void workking() = 0;
    //这是线程获取自身线程id 的函数 
    pthread_t getThreadId()const{

        // 如果这么写 return pthread_self();你就too young 了 ，呵呵
        // 当别的线程调用本线程的这个函数时 ，pthread_self()返回的不是本线程的pthread_t 而是 调用者的 id
        // 不要问我为什么，因为pthread_self()是系统调用 ，是返回当前占用cpu的线程的线程表中的信息，以后要注意哦，不要在naive了，呵呵
        return _tid;
    }

private:
    void setThreadId(pthread_t &tid){
        _tid = tid;
    }
    //这个成员的初始化是个棘手的问题 不能通过在构造函数初始化列表中调用 pthread_self()来初始化，原因类似上面，pthread_self()返回的是调用本类构造函数的那个线程（对于本例就是线程池的主线程）的线程id
    //于是想到了pthread_create函数通过第一个参数返回新创建线程的线程id,通过那个返回值来初始化这个成员
    pthread_t _tid;
};

#endif
