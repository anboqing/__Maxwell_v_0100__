
#ifndef _STRING_TOOL_H_
#define _STRING_TOOL_H_

#include <string>
#include <vector>
#include <strings.h> // bzero
#include <string.h> // strncpy
#include <stdlib.h> // mblen

using namespace std;

#define _ILEAGAL -1
#define _IS_NULL 0
#define _IS_ASCIII 1
#define _IS_GBK 2
#define _IS_UTF8 3
/*
GB18030 
第1位：0x81 ~ 0xFE 1000 0001 ~ 1111 1110 
第2位：0x40 ~ 0x7E 0100 0000 ~ 0111 1110 
或 者：0x80 ~ 0xFE 1000 0000 ~ 1111 1110 
UTF-8 
第1位：0xE0 ~ 0xEF 1110 0000 ~ 1110 1111 
第2位：0x80 ~ 0xBF 1000 0000 ~ 1011 1111 
第3位：0x80 ~ 0xBF 1000 0000 ~ 1011 1111 * */

void splitIntoLetters(const std::string& key_word,std::vector<string>& letters_vec){
    //这个函数要求输入的是utf8编码
    const string::size_type len = key_word.size();
    char *arr = new char[len+1];
    bzero(arr,len+1);
    strncpy(arr,key_word.c_str(),len);
    for(string::size_type ix = 0;ix!=len;){
        if(arr[ix]<0){
            //若是utf编码（第一个字节是负数),3个字节表示一个utf8汉字
            char* chinese_letter = new char[4];
            chinese_letter[0] = arr[ix++];
            chinese_letter[1] = arr[ix++];
            chinese_letter[2] = arr[ix++];
            chinese_letter[3] = 0;
            string letter_str(chinese_letter);
            delete[] chinese_letter;
            letters_vec.push_back(letter_str);
        }else if(arr[ix]!=0){
            // 是asiII 字符
            char *letter= new char[2];
            letter[0] = arr[ix++];
            letter[1] = 0;
            string s(letter);
            letters_vec.push_back(s);
            delete[] letter;
        }else{
            //空格不要
            ++ix;
        }
    }
    delete[] arr;
}

#endif
