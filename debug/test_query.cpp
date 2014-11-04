/*************************************************************************
  > File Name: ./debug/test_query.cpp
  > Author: HunkAnn
  > Mail: hunkann@gmail.com 453775948@qq.com 
  > Created Time: Mon 03 Nov 2014 11:27:40 AM CST
 ************************************************************************/

#include<iostream>
#include "../include/Query.h"
using namespace std;

int main(){
    Query* pquery = Query::getInstance();
    string query;
    while(1){
        cout << "输入查询:"<< endl;
        cin >> query;
        string str = pquery->query(query);
        cout << str << endl;
    }
}
