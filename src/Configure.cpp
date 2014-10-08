/*************************************************************************
	> File Name: Configure.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年09月27日 星期六 23时51分51秒
 ************************************************************************/
#include "Configure.h"
#include "Log.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

#include <assert.h>

using namespace std;

//初始化静态常量
Configure* Configure::_p_conf_instance = NULL;

LockBase* Configure::_p_lock = new RWLock();

bool Configure::_loadConfigFile(const string& filename){
    //打开文件流
    ifstream ifs(filename.c_str());
    if(!ifs.is_open()){
        throw runtime_error("can not open config file");
        return false;
    }
    // 一行一行读取配置文件并且存放到_conf_map里面
    // 配置文件格式为 key value \n key value \n ...
    string line;
    while(getline(ifs,line)){
        istringstream line_stream(line);
        //判断是不是注释行 (注释行以#开头) ,空行也跳过
        if(line[0]=='#'||line.size()<2){
            continue;   //是注释行就跳过
        }
        string key,value;
        line_stream >> key >> value;
        //将读取的键值对保存在map容器中
        _conf_map[key] = value;
    }
    ifs.close();
    return true;
}

Configure::Configure(const std::string& conf_file_name){
    if(!_loadConfigFile(conf_file_name)){
        _LogError("_loadConfigFile failed!!");
        throw runtime_error("load config file error");
    }
    map<string,string>::const_iterator res;
    if((res = _conf_map.find("HOME_PATH")) != _conf_map.end()){
        _home_path = res->second;
    }
}

Configure* Configure::getInstance(){
    if(_p_conf_instance == NULL){
        // 这是用RAII管理读写锁 防止忘记解锁 造成死锁 
        // 第二个参数 0 代表 加 读锁 1 加 写锁 ，默认是0 ，由于大多是读取用，所以用读锁加锁
        LockSafeGuard raii_locker(*_p_lock);
        //_p_lock->lock();
        if(_p_conf_instance == NULL){
            // 这里写成固定的是迫不得已，以后再改
            _p_conf_instance = new Configure("../conf/config.dat");
        }
        //_p_lock->unlock();
    }
    return _p_conf_instance;
}

string Configure::getConfigByName(const string& key)const{
// 按照配置名称查找配置信息
    map<string,string>::const_iterator res;
    if( (res =_conf_map.find(key))!=_conf_map.end()){
        return _home_path+res->second;
    }else{
        throw runtime_error("没有这样的配置信息");
        return string();
    }
}
