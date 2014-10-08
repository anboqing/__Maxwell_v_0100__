/*************************************************************************
	> File Name: ../src/Index.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年10月08日 星期三 15时14分40秒
 ************************************************************************/
#include "../include/Index.h"
#include "../include/Log.hpp"
#include "../include/Configure.h"
#include <sstream>

using namespace std;
//  初始化静态成员变量
Index* Index::_s_p_instance = NULL;
MutexLock* Index::_s_p_lock = new MutexLock();

Index::Index():_index_hashmap(){
    if(!loadIndex()){
        _LogError("读取索引错误");
        exit(EXIT_FAILURE);
    }
}

Index::~Index(){
    delete _s_p_lock;
    _s_p_lock = NULL;
    delete _s_p_instance;
    _s_p_instance = NULL;
}

Index* Index::getInstance(){
    if(_s_p_instance==NULL){
        LockSafeGuard raii(*_s_p_lock);
        if(_s_p_instance==NULL){
            _s_p_instance = new Index();
        }
    }
    return _s_p_instance;
}

// 从索引文件中读取数据到_index_hashmap;
bool Index::loadIndex(){
    // 读取配置文件
    Configure* pconf = Configure::getInstance();
    string index_path = pconf->getConfigByName("INDEX_PATH");
    ifstream ifs(index_path.c_str());
    if(!ifs.is_open()){
        _LogError("打开索引文件错误");
        return false;
    }
    string key_word;
    set<size_t> indexes_set;
    string line;
    while(getline(ifs,line)){
        key_word.clear();
        indexes_set.clear();
        istringstream iss(line);
        iss >> key_word;
        size_t index;
        while(iss >> index){
            indexes_set.insert(index);
        }
        _index_hashmap.insert(make_pair(key_word,indexes_set));
    }
    ifs.close();
    return true;
}

Index::_INDEX_MAP_TYPE& Index::getIndexMap(){
    return _index_hashmap;
}
