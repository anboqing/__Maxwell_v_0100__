/*************************************************************************
	> File Name: Configure.h
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年09月27日 星期六 22时50分00秒
 ************************************************************************/

#include "Uncopyable.h"
#include "Lock.h"

#include <map>
#include <string>
#include <stdexcept>

#ifndef _CONFIGURE_H_
#define _CONFIGURE_H_

class Configure:public Uncopyable{
    //把本例实现为线程安全的单例模式
public:
    static Configure* getInstance();
    //按照配置名获取配置信息
    std::string getConfigByName(const std::string& conf_name)const;
private:
    Configure(const std::string& conf_file_name);
    ~Configure(){}
   //存储配置信息的Map （配置名，配置值）
    std::map<std::string,std::string> _conf_map;
//    static std::shared_ptr<Configure *> _sp_conf_instance;
    static Configure* _p_conf_instance;
    // 用来保证线程安全
    static MutexLock* _p_lock;
    // 按照文件名读取配置文件的函数
    bool _loadConfigFile(const std::string& conf_file_name);
};

#endif
