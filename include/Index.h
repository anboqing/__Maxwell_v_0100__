/*************************************************************************
	> File Name: Index.h
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年10月08日 星期三 14时00分04秒
 ************************************************************************/
#ifndef _INDEX_H_
#define _INDEX_H_

#include <unordered_map>
#include <set>
#include <string>

#include "Lock.h"
#include "Uncopyable.h"

class Index:public Uncopyable{
public:
    // unordered_map 是 c++ 11 对 hashmap的实现 
    //unordered_map containers are faster than map containers to access individual elements by their key,
    // although they are generally less efficient for range iteration through a subset of their elements.
    // hashmap比 map查询效率高，所以选hashmap
    typedef std::unordered_map<std::string,std::set<size_t> > _INDEX_MAP_TYPE;
    static Index* getInstance();
    _INDEX_MAP_TYPE& getIndexMap();
    bool getIndexes(const std::string&,std::set<size_t>&);
private:
    Index();
    ~Index();
    bool loadIndex();
    _INDEX_MAP_TYPE _index_hashmap;
    static Index* _s_p_instance;
    static MutexLock* _s_p_lock;
};

#endif
