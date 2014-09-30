#ifndef _MAXWELL_LOG_H_
#define _MAXWELL_LOG_H_

#include <time.h>
#include <string.h> // strrchr
#include <string>
#include <cstdarg>  // vsnprintf
#include <cassert>
#include <fstream>
#include <stdexcept>
#include <iostream>

#include "Configure.h"
#include "Uncopyable.h"

// 返回文件基地址的宏（不能改为inline函数是因为宏中用到__FILE__宏）
// 若路径中有'/' 就返回 '/' 后的位置 .否则就是 __FILE__
//static void Logging(size_t level,const std::string& filename,int lineno,const std::string& fmt,...){
#define FILE_BASENAME strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__

#define LogDebug(fmt, ...) ABQ::Log::Logging(ABQ::_LEVEL_DEBUG, FILE_BASENAME, __LINE__, fmt, ## __VA_ARGS__)

#define LogInfo(fmt, ...) ABQ::Log::Logging(ABQ::_LEVEL_INFO, FILE_BASENAME, __LINE__, fmt, ## __VA_ARGS__)

#define LogWarn(fmt, ...) ABQ::Log::Logging(ABQ::_LEVEL_WARN, FILE_BASENAME, __LINE__, fmt, ## __VA_ARGS__)

#define LogError(fmt, ...) ABQ::Log::Logging(ABQ::_LEVEL_ERROR, FILE_BASENAME, __LINE__, fmt, ## __VA_ARGS__)

#define LogFatal(fmt, ...) ABQ::Log::Logging(ABQ::_LEVEL_FATAL, FILE_BASENAME, __LINE__, fmt, ## __VA_ARGS__)

namespace ABQ{
// 日志等级数组
static const char * _LOG_LEVEL_ARRAY[5] = {"DEBUG","INFO","WARN","ERROR","FATAL"};
// 日志格式字符串 (时间，文件名，行号，级别，msg)
static const char * _LOG_FORMAT = "%s %s:%d %s %s\n";
// 日志时间格式字符串
static const char * _LOG_TIME_FORMAT = "%Y-%m-%d %H:%M:%S"; 
//日志级别常量
static const int _LEVEL_DEBUG = 0;
static const int _LEVEL_INFO = 1;
static const int _LEVEL_WARN = 2;
static const int _LEVEL_ERROR = 3;
static const int _LEVEL_FATAL = 4;
static const int _CSTR_BUFFER_SIZE = 32;
class Log:public Uncopyable{
public:
    static void Logging(size_t level,const std::string& filename,int lineno,const std::string& fmt,...){
        //判断需要的日志等级，如果小于指定的等级就不记录
        // 这样就可以在编译选项中加入 -DLOG_LEVEL 来选择输出的日志等级
#ifdef LOG_LEVEL
        if(level<LOG_LEVEL)return;
#endif
        // assert 可通过 -D NDEBUG去掉
        assert(level<=_LEVEL_FATAL);
        std::string msg;
        size_t msg_size = 256;
        //变长参数结构 （见有道 笔记）
        va_list ap;
        //将所有参数按照格式化字符串 fmt 格式化到 msg 对象中
        for(;;){
            msg.resize(msg_size);
            va_start(ap,fmt);
            int n = vsnprintf((char *)msg.c_str(),msg_size,fmt.c_str(),ap);
            va_end(ap);
            if(n > -1 && n < (int)msg_size){
                msg.resize(n);
                break;
            }
            if(n > -1){
                msg_size = n + 1;
            }else{
                msg_size *= 2;
            }
        }
        //准备时间
        char time_str[_CSTR_BUFFER_SIZE];
        time_t timeNow;
        time(&timeNow);
        strftime(time_str, sizeof(time_str), _LOG_TIME_FORMAT, localtime(&timeNow));
        /*
        size_t strftime(char *s, size_t max, const char *format const struct tm *tm);
        The  strftime()  function  formats  the  broken-down time tm according to the format 
        specification format and places the result in the character array s of size max.
        */

        // 现在把日志写到日志文件里去
        // 1 获取日志文件地址
        Configure* pconf = Configure::getInstance();
        std::string log_path = pconf ->getConfigByName("LOG_PATH");
        std::ofstream ofs(log_path.c_str(),std::ofstream::app);
        if(!ofs.is_open()){
            throw std::runtime_error("log_path is incorrectly \n");
        }
        // 输出格式 
        int res_size = 256;
        std::string res_str;
        for(;;){
            res_str.resize(res_size);
            int iret = sprintf((char*)res_str.c_str(),_LOG_FORMAT,time_str,filename.c_str(),lineno,_LOG_LEVEL_ARRAY[level],msg.c_str());
            if(iret>=0&&iret<res_size){
                res_str.resize(iret);
                break;
            }
            if(iret>=res_size){
                res_size = iret+1;
            }else{
                res_size*=2;
            }
        } 
        ofs << res_str << std::endl;
        ofs.close();
    }
};
}
#endif
