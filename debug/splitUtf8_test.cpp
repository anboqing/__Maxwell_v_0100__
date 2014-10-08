/*************************************************************************
	> File Name: ../debug/splitUtf8_test.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年10月08日 星期三 17时36分06秒
 ************************************************************************/
#include <string>
#include <iostream>
#include <vector>
#include <strings.h>
#include <string.h>
#include <stdio.h>

using namespace std;

void splitIntoLetters(const string& key_word,vector<string>& letters_vec){
    const string::size_type len = key_word.size();
    char *arr = new char[len+1];
    bzero(arr,len+1);
    strncpy(arr,key_word.c_str(),len);
    for(string::size_type ix = 0;ix!=len;){
        printf("arr[%d] : %d",(int)ix,arr[ix]);
        if(arr[ix]<0){
            //若是UTF编码（第一个字节是负数)
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

int main(){
    vector<string> vec;
    string str("一万年");
    splitIntoLetters(str,vec);
    cout << vec.size()<< endl;
    for(auto x:vec){
        cout << x << endl;
    }
}
