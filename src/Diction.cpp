/*************************************************************************
	> File Name: ../src/Diction.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年10月08日 星期三 10时37分47秒
 ************************************************************************/
#include "../include/Diction.h"
#include "../include/Log.hpp"
#include "../include/Configure.h"

#include <utility>
#include <string.h>
#include <errno.h>
#include <fstream>
#include <stdexcept>
#include <unistd.h>
#include <stdlib.h>
#include <sstream>

using namespace std;

//初始化静态成员变量
Diction* Diction::_s_p_instance = NULL;
MutexLock* Diction::_p_lock = new MutexLock();

Diction* Diction::getInstance(){
    if(_s_p_instance==NULL){
        _p_lock->lock();
        if(_s_p_instance==NULL){
            _s_p_instance = new Diction();
        }
        _p_lock->unlock();
    }
    return _s_p_instance;
}

Diction::Diction():_dict_vec(){
    if(!loadDiction()){
        _LogError("load Diction Error");
        exit(EXIT_FAILURE);
    }
}

Diction::~Diction(){
    delete _p_lock;
    delete _s_p_instance;
    _p_lock = NULL;
    _s_p_instance = NULL;
}

//从词典文件中读取数据到vector中
bool Diction::loadDiction(){
    // 读取配置
    Configure* pconf = Configure::getInstance();
    string mydict_path = pconf->getConfigByName("MY_DICT_PATH");
    ifstream ifs;
    ifs.open(mydict_path.c_str());
    if(!ifs.is_open()){
        _LogError("文件打开失败%s",strerror(errno));
    }
    // 把数据读取到 _dict_vec 
    string line;
    string key_word;
    size_t frequency;
#ifdef DEBUG
    int count = 0;
#endif
    while(getline(ifs,line)){
#ifdef DEBUG
        ++count;
#endif
        // 这里要考虑空行的情况，还要考虑数据格式不符合规定的情况
        istringstream iss(line);
        if(!(iss>>key_word && iss>> frequency)){
#ifdef DEBUG
            cout << "第 " << count << "行 数据格式不正确"<< endl;
#endif
            return false;
        }
        _dict_vec.push_back(make_pair(key_word,frequency));
    }
    ifs.close();
    return true;
}

Diction::_DICT_VEC_TYPE& Diction::getDictionVec(){
    return _dict_vec;
}
