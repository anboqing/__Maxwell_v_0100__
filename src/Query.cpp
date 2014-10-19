/*************************************************************************
	> File Name: ../src/Query.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年10月09日 星期四 22时00分10秒
 ************************************************************************/
#include "../include/Query.h"
#include "../include/Index.h"
#include "../include/string_tools.h"
#include "../include/Diction.h"
#include "../include/edit_distance.h"
#include "../include/Log.hpp"
#include <queue>

using namespace std;

Query::Query(){}
Query::~Query(){}
void Query::getSimWords(const string& keyword,Query::_RESULT_VEC_TYPE& res_vec){
    /* 从索引中查找 
     * 1. 把关键词切分成单个汉字或字母存入一个vector<string>
     * 2. 在索引中查找每个字在索引中的下标，并把本关键词的所有字的索引项存起来set<size_t>，这样就可以通过set中的下标找到该单词在词库中对应的词（候选词）
     * 3. 计算关键词和所有候选词的编辑距离，并且把计算结果加入小根堆中，建堆的时候按照编辑距离建堆，这样堆的最顶端一直是编辑距离最小的候选词，也就是和关键词最相近的候选词
    */    
    priority_queue<HeapData4Query> small_root_heap;
    // @_1
    vector<string> keyword_letter_vec; // 存储关键词分出来的单个字
    splitIntoLetters(keyword,keyword_letter_vec);
#ifdef DEBUG
    _LogDebug("keyword_letter_vec.size() %d ",keyword_letter_vec.size());
#endif
    // 获取索引数据
    Index* p_index = Index::getInstance();
    set<size_t> indexes_set; // 存储所有候选词在词典中的下标
    // @_2.查索引
    for(auto& aletter : keyword_letter_vec){
        // 查每个单字在索引中的索引,存到下标集合中
        p_index->getIndexes(aletter,indexes_set);
    }
#ifdef DEBUG
    _LogDebug("index_set.size() %d ",indexes_set.size());
#endif
    // 准备词库
    Diction* p_diction = Diction::getInstance();
    Diction::_DICT_VEC_TYPE& diction_vec = p_diction->getDictionVec();
    //@_3
    for(auto& index : indexes_set){
        // 遍历每个下标，取出词库中对应下标的词，逐一和查询词计算编辑距离
        // 结果存入 优先级队列（小根堆）中
        string& candidite = diction_vec[index].first;
        // 计算编辑距离
        int distance = getEditDistance(candidite,keyword);
        // 构造优先级队列的数据元素并存储（建堆）
        HeapData4Query data(distance,index);
        small_root_heap.push(data);
    }
#ifdef DEBUG
    _LogDebug("small_root_heap.size() %d ",small_root_heap.size());
#endif
    // 把优先级队列的前 top_k 个词作为结果返回
    int count = 4;
    while(count--&&!small_root_heap.empty()){
        res_vec.push_back(diction_vec[small_root_heap.top()._index]);
        small_root_heap.pop();
    }
}
