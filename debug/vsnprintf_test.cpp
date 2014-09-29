/*************************************************************************
	> File Name: vsnprintf_test.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年09月28日 星期日 15时54分20秒
 ************************************************************************/
#include <stdio.h>
#include <string>
#include <stdarg.h>
#include <iostream>

using namespace std;

void func(string& fmt,...){
    int n;
    int size = 5;
    string msg;
    va_list ap;
    msg.resize(size);
    va_start(ap,fmt);
    n = vsnprintf((char*)msg.c_str(),size,fmt.c_str(),ap);
    va_end(ap);
    cout <<"n "<<  n << endl;
    cout <<"msg : "<<  msg << endl;
    cout <<"msg.size() " << msg.size() << endl;
}
int main(){
    string fmt("%d%s");
    func(fmt,3,"helloworld");
}
