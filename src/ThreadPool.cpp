/*************************************************************************
  > File Name: ../src/ThreadPool.cpp
  > Author: HunkAnn
  > Mail: hunkann@gmail.com 453775948@qq.com 
  > Created Time: 2014年09月29日 星期一 23时55分07秒
 ************************************************************************/
#include "ThreadPool.h"
#include "WorkThread.h"
#include "Log.hpp"

using namespace std;
using namespace ABQ;

ThreadPool::ThreadPool(const _TYPE_VECSIZE& vec_size):_thread_vec(vec_size)
                                                      ,_task_queue(),
                                                      _mutex_lock()
                                                      ,_condition(_mutex_lock)
                                                      ,_is_pool_open(false){
    // 给线程池中的每个工作线程 “ 注册” 自身的指针（告诉工作线程可以用这个指针来操作线程池的任务队列）
    for(vector<WorkThread>::iterator iter = _thread_vec.begin(); iter!=_thread_vec.end();++iter){
        (*iter).regeditPool(this);
    }
}

ThreadPool::~ThreadPool(){
    closePool();
}

void ThreadPool::openPool(){
    if(_is_pool_open == true){
        return;
    }
    for(vector<WorkThread>::iterator iter = _thread_vec.begin();iter!=_thread_vec.end();++iter){
        (*iter).start();
    }
    _is_pool_open = true;
}


void ThreadPool::closePool(){
    if(!_is_pool_open){
        return;
    }
    
    // 先唤醒所有阻塞的线程
    _condition.wakeUpAll();

    _mutex_lock.lock();
    //这个标志的设置和上面 wakeUpAll的顺序影响程序运行结果
    _is_pool_open = false;
    _mutex_lock.unlock();

    for(vector<WorkThread>::iterator iter = _thread_vec.begin();iter!=_thread_vec.end();++iter){
        //这里这样调用会出问题 因为 主线程要 join 别的线程，必须指定别的线程的 id
        //(*iter)->join();
        pthread_t tid = (*iter).getThreadId();
        //这句引起了 core dump 系统发出了SIGABRT信号 #########
        //(*iter)->exit();
        int iret = pthread_join(tid,NULL);
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
                LogInfo("线程%d 正常终止",tid);
                break;
        }

        while(_task_queue.size()){
            _task_queue.pop();
        }
    }
}

bool ThreadPool::addTask(Task t){
    //向任务队列加任务 
    // 要求线程安全
    //先检查线程池是否已经开启(这里就要求加锁保证不会读取到脏数据)
    //如果线程池以关闭
#ifdef DEBUG
    LogInfo("addTask");
#endif
    _mutex_lock.lock();
    if(!_is_pool_open){
        _mutex_lock.unlock();
        LogError("%s","addTask 时 线程池是关闭的");
        return false;
    }
    //添加任务到任务队列
    _task_queue.push(t);
    //唤醒一个阻塞等待任务的工作线程
    _condition.wakeUp();
#ifdef DEBUG
        LogInfo("addTask 唤醒 了 在condition上阻塞的线程");
#endif
    _mutex_lock.unlock();
    return true;
}

bool ThreadPool::getTask(Task *return_task){
    // 从任务队列中取出一个任务 到return_task 中
    // 当然也是要求线程安全的
    // 先查看任务队列是否为空
    _mutex_lock.lock();
 #ifdef DEBUG
    LogInfo("getTask start --------------- ！！！！！！！！");
#endif
   if(!_is_pool_open){
        LogError("%s","getTask时线程池是关闭的");
        _mutex_lock.unlock();
        return false;
    }
    // _task_queue.size()是线程安全的吗 #################
    while(_task_queue.size()==0){
        //队列为空 就 阻塞等待 并且解锁
#ifdef DEBUG
        LogInfo("getTask 时阻塞在condition上");
#endif
        _condition.wait();
#ifdef DEBUG
        LogInfo("从阻塞中唤醒");
#endif
    }
    // 队列不空的话就从队列中取出一个任务
    *return_task = _task_queue.front();
    _task_queue.pop();
    _mutex_lock.unlock();
#ifdef DEBUG
    LogInfo("getTask end ------------ ！！！！！！！！");
#endif
    return true;
}
