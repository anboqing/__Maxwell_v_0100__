/*************************************************************************
	> File Name: Lock.h
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年09月27日 星期六 23时21分28秒
 ************************************************************************/

#include <pthread.h>
#include "Uncopyable.h"
#include <iostream>

#ifndef _MAXWELL_MUTEXLOCK_H_
#define _MAXWELL_MUTEXLOCK_H_

class MutexLock:public Uncopyable{
public:
    MutexLock(){
        pthread_mutex_init(&_mutex,NULL);
    }
    ~MutexLock(){
        pthread_mutex_destroy(&_mutex);
    }
    void lock(){
        pthread_mutex_lock(&_mutex);
    }
    void unlock(){
        pthread_mutex_unlock(&_mutex);
    }
private:
    pthread_mutex_t _mutex;
};

class Lock:public Uncopyable{
//本类是一个RAII（resource acquisition is initialization)作用的包装类，用于将MutexLock类的对象在合适的时候解锁，防止忘记解锁
public:
    explicit Lock(MutexLock& lock):_lock(lock){
        _lock.lock();
    }
    ~Lock(){
#ifdef DEBUG
        std::cout << "lock 自动解锁" << std::endl;
#endif
        _lock.unlock();
    }
private:
    MutexLock& _lock;
};

#endif
