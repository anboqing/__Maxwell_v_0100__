/*************************************************************************
> File Name: Lock.h
> Author: HunkAnn
> Mail: hunkann@gmail.com 453775948@qq.com 
> Created Time: 2014年09月27日 星期六 23时21分28秒
************************************************************************/


#ifndef _MAXWELL_MUTEXLOCK_H_
#define _MAXWELL_MUTEXLOCK_H_

#include <pthread.h>
#include "Uncopyable.h"
#include <iostream>
#include <cstdlib>

class LockBase:public Uncopyable{
public:
    LockBase(){}
    virtual ~LockBase(){}
    virtual int lock(const int flag = _READLOCK) = 0;
    virtual int unlock() = 0;
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
    int lock(const int flag = _READLOCK){
        return  pthread_mutex_lock(&_mutex);
    }
    int unlock(){
        return pthread_mutex_unlock(&_mutex);
    }
    int tryLock(){
        return pthread_mutex_trylock(&_mutex);
    }
private:
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
    int lock(const int flag = _READLOCK){
        if(flag==_READLOCK){
            return readLock();
        }else if(flag==_WRITELOCK){
            return writeLock();
        }
        return EXIT_FAILURE;
    }
    int readLock(){
        return pthread_rwlock_rdlock(&_rw_lock);
    }
    int writeLock(){
        return pthread_rwlock_wrlock(&_rw_lock);
    }
    int unlock(){
        return pthread_rwlock_unlock(&_rw_lock);
    }
    int tryReadLock(){
        return pthread_rwlock_tryrdlock(&_rw_lock);
    }
    int tryWriteLock(){
        return pthread_rwlock_trywrlock(&_rw_lock);
    }
private:
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
