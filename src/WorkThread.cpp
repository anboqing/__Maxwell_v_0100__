/*************************************************************************
  > File Name: ../src/WorkThread.cpp
  > Author: HunkAnn
  > Mail: hunkann@gmail.com 453775948@qq.com 
  > Created Time: 2014年09月29日 星期一 23时26分49秒
 ************************************************************************/
#include "WorkThread.h"
#include "Task.h"
#include "ThreadPool.h"
#include <iostream>
#include "Log.hpp"

using namespace std;

void WorkThread::workking(){
    //从任务队列中取得任务并执行
    Task t;
    LogDebug("working running");
    while(1){
        if(_p_pool->getTask(&t)){
            cout << "t._lo is " <<  t._lo + t._ro << endl;
        }else{
            cout << "getTask false" << endl;
        }
    }
}
void WorkThread::regeditPool(ThreadPool* p_pool){
    _p_pool = p_pool;
}
