/*************************************************************************
	> File Name: ../debug/st_iterator_test.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: Tue 21 Oct 2014 09:36:56 AM CST
 ************************************************************************/

#include<iostream>
#include <set>
#include <utility>
using namespace std;

int main(){
    set<pair<int,int> > st;
    for(set<pair<int,int> >::iterator iter = st.begin(); iter!= st.end(); ++iter){
        iter->second = 2;
    }
    return 0;
}
