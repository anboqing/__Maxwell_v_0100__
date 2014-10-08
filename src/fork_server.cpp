/*************************************************************************
  > File Name: fork_server.cpp
  > Author: HunkAnn
  > Mail: hunkann@gmail.com 453775948@qq.com 
  > Created Time: 2014年10月03日 星期五 15时48分51秒
 ************************************************************************/

#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>

#include "../include/Task.h"
#include "../include/ThreadPool.h"
#include "../include/Log.hpp"

using namespace ABQ;
using namespace std;

#define BACKLOG 10240


void excute(int fd,ThreadPool&);
//void excute(int fd);

void waitChild(int);

int main(){

    int listenfd,connfd;
    struct sockaddr_in new_client_addr,server_addr;
    socklen_t  addrlen;

    if((listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        _LogError("createSocket error %s",strerror(errno));
        exit(EXIT_FAILURE);
    }

    bzero(&server_addr,sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(9987);

    addrlen = sizeof(server_addr);

    if( (bind(listenfd,(struct sockaddr*)&server_addr,addrlen))<0 ){
        _LogError(" bind listen socket error %s",strerror(errno));
        exit(EXIT_FAILURE);
    }

    if( listen(listenfd,BACKLOG)<0 ){
        _LogError(" listen socket error %s",strerror(errno));
        return EXIT_FAILURE;
    }

    //如果写在这里的话那么pool在fork时候复制了一份  会导致很单疼的结果
    // ThreadPool pool(1);
    // pool.openPool();

    struct sigaction _sigaction;
    //为了避免子进程成为僵尸进程，所以这里要捕捉 SIGCHLD  信号并且waitpid之
    _sigaction.sa_handler = waitChild;
    sigemptyset(&_sigaction.sa_mask);
    _sigaction.sa_flags = 0;

    sigaction(SIGCHLD,&_sigaction,0);

    pid_t pid;
    for(;;){
        addrlen = sizeof(struct sockaddr_in);
        if( (connfd = accept(listenfd,(struct sockaddr*)&new_client_addr,&addrlen))<0 ){
            if(errno==EINTR){
                continue; //重启被中断的慢系统调用
                            // 如果accept期间捕获一个子进程结束的信号，那么accept返回-1并且errno为EINTR，所以要处理
            }else{
                _LogError(" listen socket error %s",strerror(errno));
            }
        }
        pid = fork();
        if(pid==0){
            /*child*/
            close(listenfd);
            // 处理客户发来的请求
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            // 血泪啊，这里调试了4个小时啊
            // 如果不写在这里（让每个子进程都有一个pool)的话，写到外面，那fork()之后 主进程和子进程
            // 都有同样一个ThreadPool 主进程 addTask 是往自己的 taskqueue中add ,而子进程的taskqueue中就没有东西
            // 而且主进程addTask中__condition.wakeUp()不会唤醒子进程中阻塞在__condition上的线程
            // 妈蛋！！！！！！！！！！！！！
            ThreadPool pool(1);
            pool.openPool();
            excute(connfd,pool);
            exit(0);
        }
        /* parent */
        close(connfd);
    }
}

void excute(int fd,ThreadPool& pool){
    // 从fd 中读取2个数字 封装到 Task中 然后 addTask(Task t);
    char buf[512];
    ssize_t readn;
    Task t;
    for(;;){
again:
        bzero(buf,512);
        if( (readn = read(fd,buf,512)) > 0 ) {
            buf[readn] = 0;
            if(sscanf(buf,"%d %d",&t._lo,&t._ro) < 2){
                _LogWarn(" 输入格式错误 ");
                break;
            }
        }
        if(readn<0 && errno == EINTR){
            goto again;
        }else if(readn<0){
            _LogError("server read error %s",strerror(errno));
            return;
        }
        t._sock_fd = fd;
        //bzero(buf,512);
        //sprintf(buf,"%d",t._lo+t._ro);
        //send(fd,buf,strlen(buf),0);
        if(!pool.addTask(t)){
            _LogWarn("add Task false");
            exit(-1);
        }
    }
}

void waitChild(int signo){
    pid_t pid;
    int status;
    while( (pid = waitpid(-1,&status,WNOHANG))>0 ){
#ifdef DEBUG
        LogInfo(" 子进程 %d 终止",pid);
#endif
    }
    return;
}
