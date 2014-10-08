/*************************************************************************
	> File Name: ../debug/Diction_Test.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年10月08日 星期三 12时41分52秒
 ************************************************************************/
#include "../include/Diction.h"
#include <iostream>
using namespace std;

int main(){
    Diction *pdict = Diction::getInstance();
    Diction::_DICT_VEC_TYPE& dict =  pdict->getDictionVec();
    cout << dict[1000].first << endl;
    return 0;
}

// g++ Diction_Test.cpp ../src/Diction.cpp ../src/Configure.cpp -I../include -lpthread -DDEBUG 
