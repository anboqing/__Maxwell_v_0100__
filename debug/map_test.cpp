/*************************************************************************
	> File Name: ../debug/map_test.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: Tue 21 Oct 2014 11:00:02 AM CST
 ************************************************************************/

#include<iostream>
#include <map>
using namespace std;

int main(){
    map<size_t,double> mp;
    for(int i = 0;i<10;++i){
        //mp[i]+=0.124;
        mp[i];
    }
    for(auto& pair:mp){
        cout << pair.second << endl;
    }
}

