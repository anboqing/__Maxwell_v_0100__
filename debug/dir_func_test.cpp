/*************************************************************************
	> File Name: dir_func_test.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: Wed 15 Oct 2014 04:02:35 PM CST
 ************************************************************************/

#include<iostream>
#include <string.h>
#include <unistd.h>

using namespace std;

int main(){
    char buf[512];
    int len = 512;
    cout << "1-- " <<  get_current_dir_name() << endl;
    getcwd(buf,len);
    cout << "2-- "<< buf << endl;
}
