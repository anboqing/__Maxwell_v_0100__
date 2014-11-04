/*************************************************************************
	> File Name: Query.h
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: Wed 22 Oct 2014 09:01:45 AM CST
 ************************************************************************/

#ifndef _QUERY_H_
#define _QUERY_H_

#include "Uncopyable.h"
#include "Lock.h"
#include "DocIndex.h"
#include "Document.h"
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include "CppJieba/HMMSegment.hpp"
#include "CppJieba/MixSegment.hpp"

class ResData{
// 用来对计算出的文档的相似度进行排序
public:
    ResData(double s,std::size_t id):_simi(s),_docid(id){}
    double _simi;
    std::size_t _docid;
    friend bool operator<(const ResData& lo,const ResData& lr){
        return lo._simi > lr._simi;
    }
};

class Query:public Uncopyable{
public:
    // 查询并获取结果集合的json字符串
    std::string query(const std::string& );
    static Query* getInstance();
private:
    Query();
    ~Query();
    static Query* _s_p_instance;
    static MutexLock* _s_lock;
    static DocIndex* _inverted_index;
};

#endif
