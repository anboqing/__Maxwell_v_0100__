/*************************************************************************
	> File Name: ../src/DocIndex.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: Mon 20 Oct 2014 04:18:49 PM CST
 ************************************************************************/

#include "../include/DocIndex.h"
#include <stdexcept>
#include <sstream>
#include <fstream>
#include "../include/Configure.h"

using namespace std;

DocIndex* DocIndex::_s_p_instance = NULL;
MutexLock* DocIndex::_s_p_lock = new MutexLock();

DocIndex::DocIndex(){
    if(!loadIndex()){
        throw runtime_error("读取文档索引错误");
    }
}

DocIndex::~DocIndex(){
    delete _s_p_lock;
    _s_p_lock = NULL;
    delete _s_p_instance;
    _s_p_instance = NULL;
}

DocIndex* DocIndex::getInstance(){
    if(_s_p_instance==NULL){
        LockSafeGuard lock_manager(*_s_p_lock);
        if(_s_p_instance == NULL){
            _s_p_instance = new DocIndex();
        }
    }
    return _s_p_instance;
}

// 从索引文件中读取数据到_index_hashmap;
bool DocIndex::loadIndex(){
    Configure* pconf = Configure::getInstance();
    string doc_index_path = pconf->getConfigByName("DOC_INDEX_PATH");
    ifstream index_reader(doc_index_path.c_str());
    if(!index_reader.is_open()){
        throw std::runtime_error("打开文档索引文件错误");
        return false;
    }
    string key_word;
    unordered_map<size_t,double> indexes_map;
    string line;
    while(getline(index_reader,line)){
        key_word.clear();
        indexes_map.clear();
        istringstream line_parser(line);
        line_parser >> key_word;
        size_t index;
        double weight;
        while(line_parser>> index && line_parser>> weight){
            indexes_map.insert(make_pair(index,weight));
        }
        _index_hash_map.insert(make_pair(key_word,indexes_map));
    }
    index_reader.close();
    return true;
}

DocIndex::_DOC_INDEX_MAP_TYPE& DocIndex::getIndexMap(){
    return _index_hash_map;
}

bool DocIndex::getIndexes(const std::string& keyword,std::unordered_map<size_t,double>& result_map){
    _DOC_INDEX_MAP_TYPE::iterator res_iter;
    if( (res_iter = _index_hash_map.find(keyword)) == _index_hash_map.end() ){
        // 没找到
        return false;
    }
    unordered_map<size_t,double>& result = res_iter->second;
    // 这里不能直接赋值，要append到结果set里面
    for(auto& pair: result){
        result_map.insert(pair);
    }
    return true;
}

