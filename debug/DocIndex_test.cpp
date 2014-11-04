/*************************************************************************
	> File Name: ../debug/DocIndex_test.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: Wed 22 Oct 2014 10:10:31 AM CST
 ************************************************************************/

#include<iostream>

#include "../include/DocIndex.h"

using namespace std;

int main(){
    DocIndex* pindex = DocIndex::getInstance();
    DocIndex::_DOC_INDEX_MAP_TYPE& idx_map = pindex->getIndexMap();
    cout << idx_map.size() << " ------ " << endl;
    return 1;
}
