/*************************************************************************
	> File Name: fork_client.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年10月03日 星期五 17时29分43秒
 ************************************************************************/
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <errno.h>
#include <string.h>
#include <iostream>

#include "../include/Log.hpp"



int main(){

    struct sockaddr_in server_addr;
    socklen_t addrlen = sizeof(server_addr);
    int fd;

    // make socket
    if( (fd = socket(AF_INET,SOCK_STREAM,0)) < 0 ){
        LogError("client socket error %s",strerror(errno));
        return -1;
    }

    // connect
    server_addr.sin_port = htons(9987);
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET,"127.0.0.1",&server_addr.sin_addr);
    if( connect(fd,(struct sockaddr*)&server_addr,addrlen)<0 ){
        LogError("connect error %s",strerror(errno));
        return -1;
    }
    // 收发数据
    char buf[512];
    using namespace std;
    int lo,ro;
    for(;;){
        //清空缓冲区
        bzero(buf,512);
        cout << "请输入2个数字，服务器将为你球和" << endl;   
        cin >> lo >> ro;
        if(ro==0){
            break;
        }
        //把 lo ro 格式化进 buf
        sprintf(buf,"%d %d",lo,ro);
        buf[strlen(buf)] = '\0';
        //发送数据给服务器
        if( (sendto(fd,buf,strlen(buf),0,(struct sockaddr*)&server_addr,addrlen) )== -1){
            LogError("client sendto error %s",strerror(errno));
        }
        //从服务器读取结果
        bzero(buf,512);
        cout << " 要接数据了 "<< endl;
        if( recv(fd,buf,512,0)==-1 ){
            LogError("client recv error %s",strerror(errno));
        }
        cout << " 接受到的结果为 " << buf << endl;
    }
    close(fd);
}
