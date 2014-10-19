/*************************************************************************
	> File Name: test.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年10月12日 星期日 11时50分26秒
 ************************************************************************/
#include <iostream>
#include <vector>
#include <string>

#include "../include/Configure.h"

using namespace std;

string str("hello");
string name = str.substr(0,3);
string* pname = &name;

int main(){
    cout << pname->size() << endl;
    cout << name << endl;
}
