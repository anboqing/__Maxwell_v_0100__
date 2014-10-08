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

#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>

using namespace std;
using namespace ABQ;

void excuteTask(Task &t);
void WorkThread::workking(){
#ifdef DEBUG
    LogInfo("workking");
#endif
    //从任务队列中取得任务并执行
    Task t;
    while(1){
#ifdef DEBUG
        LogInfo("阻塞在getTask上");
#endif
        if(_p_pool->getTask(&t)){
            excuteTask(t);
        }else{
            LogError("%s","getTask false");
            break;
        }
    }
    LogInfo("out of workking");
}

void excuteTask(Task &t){
#ifdef DEBUG
    LogInfo("excuteTask");
#endif
    cout << "result is " <<  t._lo + t._ro << endl;
    //把task 发送给客户端
    ssize_t size;
    char buf[512];
    bzero(buf,512);
    sprintf(buf,"%d",t._lo+t._ro);
    if( (size = send(t._sock_fd,buf,strlen(buf),0))<0 ){
        LogError("send error : %s",strerror(errno));
    }
}

void WorkThread::regeditPool(ThreadPool* p_pool){
    _p_pool = p_pool;
}
