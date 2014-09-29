/*************************************************************************
> File Name: Lock.h
> Author: HunkAnn
> Mail: hunkann@gmail.com 453775948@qq.com 
> Created Time: 2014年09月27日 星期六 23时21分28秒
************************************************************************/


#ifndef _MAXWELL_MUTEXLOCK_H_
#define _MAXWELL_MUTEXLOCK_H_

#include "Log.hpp"
#include "Uncopyable.h"

#include <string.h>
#include <iostream>
#include <pthread.h>
#include <cstdlib>

class LockBase:public Uncopyable{
public:
    LockBase(){}
    virtual ~LockBase(){}
    virtual void lock(const int flag = _READLOCK) = 0;
    virtual void unlock() = 0;
protected:
    enum{_READLOCK = 0,_WRITELOCK = 1};
};
class MutexLock:public LockBase{
public:
    MutexLock(){
        pthread_mutex_init(&_mutex,NULL);
    }
    ~MutexLock(){
        pthread_mutex_destroy(&_mutex);
    }
    void lock(const int flag = _READLOCK){
        int iret = pthread_mutex_lock(&_mutex);
        if(iret!=0){
            excuteError();
        }
    }
    void unlock(){
        int iret  = pthread_mutex_unlock(&_mutex);
        if(iret!=0){
            excuteError();
        }
    }
    void tryLock(){
        int iret = pthread_mutex_trylock(&_mutex);
        if(iret!=0){
            excuteError();
        }
    }
    pthread_mutex_t* getMutexPointer(){
        return &_mutex;
    }
private:
    void excuteError(){
        char *msg = strerror(errno);
        LogError("%s",msg);
        throw std::runtime_error(msg);
    }
    pthread_mutex_t _mutex;
};

class RWLock:public LockBase{
public:
    RWLock(){
        pthread_rwlock_init(&_rw_lock,NULL);
    }
    ~RWLock(){
        pthread_rwlock_destroy(&_rw_lock);
    }
    void lock(const int flag = _READLOCK){
        if(flag==_READLOCK){
            readLock();
        }else if(flag==_WRITELOCK){
            writeLock();
        }
    }
    void readLock(){
        int iret = pthread_rwlock_rdlock(&_rw_lock);
        if(iret != 0){
            excuteError();
        }
    }
    void writeLock(){
        int iret = pthread_rwlock_wrlock(&_rw_lock);
        if(iret != 0){
            excuteError();
        }
    }
    void unlock(){
        int iret = pthread_rwlock_unlock(&_rw_lock);
        if(iret != 0){
            excuteError();
        }
    }
    void tryReadLock(){
        int iret = pthread_rwlock_tryrdlock(&_rw_lock);
        if(iret !=0)
        {
            excuteError();
        }
    }
    void tryWriteLock(){
        int iret = pthread_rwlock_trywrlock(&_rw_lock);
        if(iret != 0){
            excuteError();
        }
    }
private:
    void excuteError(){
        char *msg = strerror(errno);
        LogError("%s",msg);
        throw std::runtime_error(msg);
    }
    pthread_rwlock_t _rw_lock;
};

class LockSafeGuard:public Uncopyable{
    //本类是一个RAII（resource acquisition is initialization)作用的包装类，用于将MutexLock类的对象在合适的时候解锁，防止忘记解锁
public:
    LockSafeGuard(LockBase& lock,const int flag = 0):_lock(lock){
        //这里我想加一个运行时类型判断，如果引用指向的是读写锁的对象，就调用 readLock() ,如果引用指向的是互斥锁，就调用互斥锁
        _lock.lock(flag);
    }
    ~LockSafeGuard(){
        _lock.unlock();
    }
private:
    LockBase& _lock;
};

#endif
