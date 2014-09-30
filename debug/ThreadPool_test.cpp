/*************************************************************************
  > File Name: ThreadPool_test.cpp
  > Author: HunkAnn
  > Mail: hunkann@gmail.com 453775948@qq.com 
  > Created Time: 2014年09月30日 星期二 01时08分00秒
 ************************************************************************/

#include "ThreadPool.h"
#include "Task.h"
#include <unistd.h>

int main(){
    ThreadPool pool(1);
    pool.openPool();
    Task t;
    t._lo = 10;
    t._ro = 20;
    while(1){
        if(!pool.addTask(t))
            break;
        sleep(1);
    }
}

/*
   g++ ThreadPool_test.cpp ../src/Configure.cpp ../src/Thread.cpp ../src/ThreadPool.cpp ../src/WorkThread.cpp -o pool.out -lpthread -I../include/ -std=c++11 -DDEBUG 
*/

