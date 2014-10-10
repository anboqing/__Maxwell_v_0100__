/*************************************************************************
	> File Name: ../debug/editdistance_test.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年10月10日 星期五 01时10分49秒
 ************************************************************************/
#include "../include/edit_distance.h"

#include "../include/EncodingConverter.hpp"

#include <string>
#include <iostream>
#include <stdio.h>
 
using namespace std;

int main(){
    string s1("金大根");
    string s2("金正恩");
    //cout << get_character_code_type((char*)&s1[0]) << endl;
    cout << getEditDistance(s1,s2) << endl;
}

/*

    string a("你");
    string b("b");
    if(!((0xe1&0xF0)^0xE0)){
        printf("%x\n",!((0xe1&0xF0)^0xE0 ));
    }else{
        printf("%d\n",!2);
        printf("%d\n",!0);
        printf("%d\n",!(-1));
    }
    printf("%x\n",0xe1&0xF0);
    printf("%x\n",((0xe1&0xF0)^0xE0 ));
    cout << get_character_code_type((char*)a.c_str())<< endl;
    cout << get_character_code_type((char*)b.c_str())<< endl;
    string c("");
    cout << get_character_code_type((char*)c.c_str())<< endl;

    string d("坏");
    string e;
    utf8_gbk(d,e);
    cout << e << endl;
    cout << get_character_code_type((char*)e.c_str()) << endl;



*/
