/*************************************************************************
	> File Name: Condition.h
    > 这个类是用来 和 互斥锁 一起使用的 
    > 允许多线程以无竞争的方式等待特定条件的发生
    > 用在线程同步中保证线程执行的先后次序
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年09月29日 星期一 18时00分07秒
 ************************************************************************/

#ifndef _MAXWELL_CONDITION_H_
#define _MAXWELL_CONDITION_H_

#include "Uncopyable.h"
#include "Lock.h"
#include "Log.hpp"

#include <pthread.h>
#include <errno.h> // errno
#include <string.h> // strerror

class Condition:public Uncopyable{
public:
    Condition(MutexLock& lock):_cond_lock(lock){
        int iret;
        if((iret=pthread_cond_init(&_condition,NULL))!=0){
            excuteError();
        }
    }
    ~Condition(){
        int iret;
        if((iret=pthread_cond_destroy(&_condition))!=0){
            excuteError();
        }
    }

    void wait(){
        int iret;
        if( (iret=pthread_cond_wait(&_condition,_cond_lock.getMutexPointer()) ) !=0){
            excuteError();
        }
    }

    void wakeUp(){
        int iret;
        if( (iret=pthread_cond_signal(&_condition)) !=0 ){
            excuteError();
        }
    }

    void wakeUpAll(){
        int iret;
        if( (iret = pthread_cond_broadcast(&_condition)) !=0 ){
            excuteError();
        }
    }

private:
    MutexLock& _cond_lock;
    pthread_cond_t _condition;   
    void excuteError(){
        char *msg = strerror(errno);
        LogError("%s",msg);
        throw std::runtime_error(msg);
    }
};

#endif

