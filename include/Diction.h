/*************************************************************************
	> File Name: Diction.h
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年10月08日 星期三 10时24分13秒
 ************************************************************************/
#ifndef _DICTION_H_
#define _DICTION_H_
#include <vector>
#include <string>

#include "Uncopyable.h"
#include "Lock.h"

/*
 * 词典类 用于运行时提供词典数据操纵
 */
class Diction:public Uncopyable{
public:
    typedef std::vector<std::pair<std::string,size_t> > _DICT_VEC_TYPE;
    static Diction* getInstance();
    _DICT_VEC_TYPE& getDictionVec();
private:
    Diction();
    ~Diction();
    //从词典文件中读取数据到内存
    bool loadDiction();
    //存放词典的
    _DICT_VEC_TYPE _dict_vec;
    static Diction* _s_p_instance;
    static MutexLock* _p_lock;
};

#endif
