/*************************************************************************
	> File Name: ../debug/encoding_test.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年10月07日 星期二 22时49分56秒
 ************************************************************************/
#include "../include/EncodingConverter.hpp"
#include <iostream>

using namespace std;

int main(){
#ifdef DEBUG
    char *inbuf = new char[3];
    strncpy(inbuf,"好",3);
     for(int i = 0; i<3;++i){
         printf("%d ",inbuf[i]);
     }
    char *outbuf = new char[3];
    char *o1 = outbuf;
    size_t inlen = sizeof("好") ,outlen = 3;
    //utf8_gbk(inbuf,inlen,outbuf,outlen);
#endif
    string hao("你好妈妈妈们妈啊");
    cout << hao << endl;
    string out;
    utf8_gbk(hao,out);
    cout << out << " " << out.size() << endl;
#ifdef DEBUG
     string ostr(o1);
     cout << ostr << endl;
     for(int i = 0; i<3;++i){
         printf("%d ",o1[i]);
     }
#endif
    return 0;
}
