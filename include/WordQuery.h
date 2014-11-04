/*************************************************************************
	> File Name: Query.h
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年10月09日 星期四 21时17分27秒
 ************************************************************************/
#ifndef _QUERY_H_
#define _QUERY_H_

#include <set>
#include <string>
#include <unordered_map> // std::unordered_map
#include <vector>   // std::vector
#include <utility> // std:: pair

#include "Uncopyable.h"

class HeapData4Query{
public:
    HeapData4Query(int dis,int index):_distance(dis),_index(index){}
    friend bool operator<(const HeapData4Query& l,const HeapData4Query& r){
        if(l._distance==r._distance){
            return false;
        }
        return l._distance > r._distance;
    }
    int _distance;
    int _index;
};

// 本类用来实现在 缓存 索引 词典中进行查询的功能
class WordQuery:public Uncopyable{
public:
    //定义一个结果集的类型
    typedef std::vector<std::pair<std::string,int> > _RESULT_VEC_TYPE;

    WordQuery();
    ~WordQuery();

    // 查询函数
    static void getSimWords(const std::string& keyword,_RESULT_VEC_TYPE&);
private:

};

#endif
