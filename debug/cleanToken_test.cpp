/*************************************************************************
	> File Name: ../debug/cleanToken_test.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年10月08日 星期三 16时20分36秒
 ************************************************************************/
#include <iostream>
#include <stdlib.h>
#include "../include/EncodingConverter.hpp"
#include "../include/CleanGBKToken.hpp"

using namespace std;

int main()
{
	string in = " *()hello,world，在过几天，我就要看完《C++程序设计》这本书了；心里很激动！想放松放松。";
    string str;
    utf8_gbk(in,str);
    cleanGBKToken(str);
    cout << str << endl;
	return 0;
}
