/*************************************************************************
	> File Name: DocIndex.h
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: Mon 20 Oct 2014 03:59:05 PM CST
 ************************************************************************/

#ifndef _DOC_INDEX_H_
#define _DOC_INDEX_H_

#include <unordered_map>
#include <string>
#include <set>
#include <utility>
#include "Lock.h"
#include "Uncopyable.h"

class DocIndex:public Uncopyable{
public:
    typedef std::unordered_map<std::string,std::set<std::pair<size_t,int> > > _DOC_INDEX_MAP_TYPE;
    static DocIndex* getInstance();
    _DOC_INDEX_MAP_TYPE& getIndexMap();
    bool getIndexes(const std::string&,std::set<std::pair<size_t,int> >& );
private:
    DocIndex();
    ~DocIndex();
    bool loadIndex();
    _DOC_INDEX_MAP_TYPE _index_hash_map;
    static DocIndex* _s_p_instance;
    static MutexLock* _s_p_lock;
};

#endif

