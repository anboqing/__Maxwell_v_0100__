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
    typedef std::vector<WorkThread>::size_type _TYPE_VECSIZE;
    ThreadPool(const _TYPE_VECSIZE&);
    ~ThreadPool();

    void openPool();
    void closePool();

    // 能不能传进去 引用或者指针
    bool addTask(Task);
    // 通过返回值返回好 还是 用参数返回好
    bool getTask(Task *);
private:
    // 工作线程 容器
    /*
     * 这里vector存放了WorkThread指针的话，那么初始化该Vector用的是 vector<type> vec(number) 这个构造函数
这个构造函数是Constructs a container with n elements. Each element is a copy of val (if provided).所以存进去的是指向相同对象的n个指针，所以不能存放指针
     * */
    std::vector<WorkThread> _thread_vec;
    // 工作任务 队列 （用引用和指针行不行，因为想传进Task的子类）
    std::queue<Task> _task_queue;
    // 互斥锁 条件变量等
    MutexLock _mutex_lock;
    Condition _condition;

    bool _is_pool_open;
};

#endif
