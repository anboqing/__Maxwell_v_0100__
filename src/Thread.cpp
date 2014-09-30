/*************************************************************************
  > File Name: ../src/Thread.cpp
  > Author: HunkAnn
  > Mail: hunkann@gmail.com 453775948@qq.com 
  > Created Time: 2014年09月29日 星期一 22时19分13秒
 ************************************************************************/
#include "Thread.h"
#include "Log.hpp"

#include <string.h>
#include <errno.h>
#include <stdexcept>

using namespace std;

//初始化列表用0初始化 _tid是因为 随后线程启动的时候 pthread_create()函数会将第一个参数设置为新建线程的id
Thread::Thread():_tid(0){}

Thread::~Thread(){
    exit();
}

void* Thread::thread_func(void *arg){
    Thread * p_thread = static_cast<Thread*>(arg);
    //这里通过指针调用了虚函数 workking 让线程按照workking 中的代码进行工作 最后 引发多态行为
    p_thread->workking();
    return NULL;
}

void Thread::start(){
    //创建线程 将本线程的指针传给线程启动函数 让它能调用 workking 函数开始工作
    // 第一个参数 将新建线程的线程id 赋值给了_tid成员
    int ret = pthread_create(&_tid,NULL,thread_func,this);
    if(ret!=0){
        LogError("%s",strerror(errno));
        throw runtime_error(strerror(errno));
    }
}

void Thread::exit(){
    pthread_exit((void*)_tid);
}

void Thread::join(pthread_t &tid){
    //下面这行代码若调用自己join自己 必定会返回 EDEADLK  
    int iret = pthread_join(tid,NULL);
    /*
       EDEADLK
       一个死锁被侦测到(如两个线程试图聚合彼此)；或 thread 指定了调用线程自身。
       EINVAL
       thread 是不一个可聚合的线程。
       EINVAL
       其它线程已经在等待聚合这个线程。
       ESRCH
       没有 ID 为 thread 的线程。
       */
    switch(iret){
        case EDEADLK:
            LogError("%s","检测到了死锁");
            break;
        case EINVAL:
            LogError("%s","thread is not a joinable thread 或者 Another thread is already waiting to join with this thread.");
            break;
        case ESRCH:
            LogError("%s","该线程id是错误的");
            break;
        default:
            break;
    }
}
