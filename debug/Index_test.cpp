/*************************************************************************
	> File Name: ../debug/Index_test.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年10月08日 星期三 17时48分42秒
 ************************************************************************/
#include "../include/Index.h"
#include <iostream>
using namespace std;
int main(){
    Index* pindex = Index::getInstance();
    cout << pindex->getIndexMap().size() << endl;
}
//g++ Index_test.cpp ../src/Configure.cpp ../src/Index.cpp ../src/Diction.cpp -lpthread -I../include/ -std=c++11
