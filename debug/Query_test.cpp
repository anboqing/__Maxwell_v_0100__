/*************************************************************************
> File Name: ../debug/Query_test.cpp
> Author: HunkAnn
> Mail: hunkann@gmail.com 453775948@qq.com 
> Created Time: 2014年10月10日 星期五 11时09分33秒
************************************************************************/
#include "../include/Query.h"
#include <vector>
#include <string>
#include <utility>
#include <iostream>

using namespace std;

int  main(){
    vector<pair<string,int> > vec;
    string key;
    while(key.clear(),cin >> key){
        Query::getSimWords(key,vec);
        for(auto& x:vec){
            cout<<x.first<<" "<<x.second<<endl;
        }
        cout << "-------------" << endl;
    }
}

/*
g++ Query_test.cpp ../src/Query.cpp ../src/Index.cpp ../src/Diction.cpp ../src/Configure.cpp -lpthread -I../include/ -DDEBUG -std=c++11
**/
