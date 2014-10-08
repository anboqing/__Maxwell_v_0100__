/*************************************************************************
	> File Name: Task.h
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年09月29日 星期一 23时34分08秒
 ************************************************************************/
#ifndef _MAXWELL_TASK_H_
#define _MAXWELL_TASK_H_

class Task{
public:
    // void* 是 out-arg
    int _lo;    // 返回的时候用于存放结果
    int _ro;   //virtual void execute(void*)  = 0;
    int _sock_fd; //用于存放 sock_file_descripter
};

#endif
