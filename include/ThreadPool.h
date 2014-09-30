/*************************************************************************
	> File Name: ThreadPool.h
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年09月29日 星期一 23时24分13秒
 ************************************************************************/

#ifndef _MAXWELL_THREADPOOL_H_
#define _MAXWELL_THREADPOOL_H_

#include <queue>
#include <vector>

#include "Task.h"
#include "Condition.h"
#include "Lock.h"
#include "WorkThread.h"


class ThreadPool:public Uncopyable{
public:
    typedef std::vector<WorkThread*>::size_type _TYPE_VECSIZE;
    ThreadPool(const _TYPE_VECSIZE&);
    ~ThreadPool();

    void openPool();
    void closePool();

    // 能不能传进去 引用或者指针
    void addTask(Task);
    // 通过返回值返回好 还是 用参数返回好
    void getTask(Task *);
private:
    // 工作线程 容器
    std::vector<WorkThread*> _thread_vec;
    // 工作任务 队列 （用引用和指针行不行，因为想传进Task的子类）
    std::queue<Task> _task_queue;
    // 互斥锁 条件变量等
    MutexLock _mutex_lock;
    Condition _condition;

    bool _is_pool_open;
};

#endif
