/*************************************************************************
	> File Name: config_test.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年09月28日 星期日 00时57分16秒
 ************************************************************************/
#include "Configure.h"
#include <iostream>

using namespace std;

int main(){
    Configure* p_conf = Configure::getInstance();
    cout << p_conf->getConfigByName("test")<< endl;
}
