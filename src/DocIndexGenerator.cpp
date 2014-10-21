/*************************************************************************
  > File Name: DocIndexGenerator.cpp
  > Author: HunkAnn
  > Mail: hunkann@gmail.com 453775948@qq.com 
  > Created Time: Mon 20 Oct 2014 04:45:39 PM CST
 ************************************************************************/

//g++ -o ../bin/DocIndexGenerator.out DocIndexGenerator.cpp Configure.cpp -I../include/ -lpthread -std=c++11 -DLOGGER_LEVEL=LL_ERROR
#include<iostream>
using namespace std;

#include "../include/CleanGBKToken.hpp"
#include "../include/Log.hpp"
#include "../include/Configure.h"
#include "../include/Document.h"
#include "../include/CppJieba/HMMSegment.hpp"
#include "../include/CppJieba/MixSegment.hpp"

#include <assert.h>
#include <unistd.h> // pause();
#include <cmath>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <string>
#include <utility>

// 生成文档倒排索引
vector<Document> doc_vec;
vector<pair<int,int> > offset_vec; // 存放所有的偏移量
set<string> _exclude_set;

bool build_offsets_vec(){
    Configure* pconf = Configure::getInstance();
    string offset_path = pconf->getConfigByName("NEW_RIPE_LIB_OFFSET_PATH"); ifstream offset_reader; 
    offset_reader.open(offset_path.c_str());
    if(!offset_reader.is_open()){
        perror("read_offset_2_vec");
        return false;
    }
    string line;
    while(getline(offset_reader,line)){
        int offset,length;
        istringstream ls(line);
        if(!( ls >> offset && ls >> length )){
            perror("偏移文件格式错误");
            return false;
        }
        offset_vec.push_back(make_pair(offset,length));
    }
    offset_reader.close();
    return true;
}

string parseTag(const string& doc_str,const char *tag,const char* tag_end){
    size_t beg = string::npos , end = string::npos;
    if( ((beg = doc_str.find(tag)) != string::npos)
            && ((end = doc_str.find(tag_end)) != string::npos)){
        beg+=strlen(tag); // 将游标移动到正确位置
        size_t len = end-beg;
        string temp(doc_str,beg,len);
        return temp;
    }else{
        _LogError("解析文档错误: tag %s tag end %s ",tag,tag_end);
        return string();
    }
}

static int doc_id = 0;

Document parseStr2Doc(string& doc_str){
    //  把 <doc> <docid></docid><url></url><title></title><content></content></doc>中的内容解析出来
    string url,title,content;
    url = parseTag(doc_str,"<url>","</url>");
    title = parseTag(doc_str,"<title>","</title>");
    content = parseTag(doc_str,"<content>","</content>");
    return *(new Document(doc_id++,url,title,content));
}

bool build_document_vec(){
    Configure* pconf = Configure::getInstance();
    string ripe_path = pconf->getConfigByName("NEW_RIPE_LIB_PATH");
    ifstream ripe_reader;
    ripe_reader.open(ripe_path.c_str());
    if(!ripe_reader.is_open() ){
        _LogError("open %s failed",ripe_path.c_str());
        return false;
    }
    if(!build_offsets_vec()){
        _LogError("build Offset vec error");
        return false;
    }
    // 读取偏移量vector 找出每篇文章的起始位置
    // 从RipePageLib中读解析出所有的Document 存入vector
    for(auto& pair : offset_vec){
        ripe_reader.seekg(pair.first,ripe_reader.beg);   
        char *buf = new char[pair.second+1];
        bzero(buf,pair.second+1);
        ripe_reader.read(buf,pair.second);
        string doc_str(buf);
        doc_vec.push_back(parseStr2Doc(doc_str));
        delete[] buf;
    }
    ripe_reader.close();
    return true;
}

void build_exclude_set(){
    //构建单词排除集
    Configure* pconf = Configure::getInstance();
    string exclude_path = pconf->getConfigByName("EXCLUDE_PATH");
    ifstream ifs(exclude_path.c_str());
    if(!ifs.is_open()){
        _LogError("open %s file failed",exclude_path.c_str());
        throw runtime_error("can not open exclude_path");
    }
    string word;
    while(ifs>>word){
        _exclude_set.insert(word);
    }
    ifs.close();
}

bool getTermFrequency(Document& doc,map<string,double>& doc_map,CppJieba::MixSegment& seg){
    string& content = doc._content;
    if( !(content.empty()) ){
        vector<string> vec_words;
        // 清理标点符号 （content 是utf8 编号) 这里得换成 清理 utf8 标点符号
        if(!clean_UTF8_Token(content)){
            _LogError("清理标点符号错误 %d",doc._docid);
            return false;
        }
        vec_words.clear();
        // 用切词器切词
        seg.cut(content,vec_words);
        map<string,int> word_fre_map;
        for(vector<string>::iterator iter = vec_words.begin();iter!=vec_words.end();++iter){
            if(!_exclude_set.count(*iter)){
                // 统计词频
                ++doc_map[*iter];
            }
        }
        // 计算 term_frequency
        vector<string>::size_type size = vec_words.size();
        for(auto& pair:doc_map){
            pair.second /= static_cast<double>(size);
        }
        return true;
    }
    return false;
}

// 词语1  docid1:weight  docid3:weight ......
// 词语2  docid8:weight  docid5:weight ......
unordered_map<string,map<size_t,double> > _index_hash_map;
unordered_map<string,map<size_t,double> > _index_hash_map_idf;
unordered_map<string,map<size_t,double> > _index_hash_map_weight;

void build_inverse_index(CppJieba::MixSegment &seg){
    // 建立倒排索引 
    for(vector<Document>::size_type ix = 0 ; ix != doc_vec.size() ; ++ ix){
        map<string,double> doc_map;
        // 对每篇文章 ： 求每个词的tf
        if(getTermFrequency(doc_vec[ix],doc_map,seg)){
            // 然后把本文章的map 合并到_index_hash_map中去
            for(auto& pair : doc_map){
                assert(pair.second > 0);
                _index_hash_map[pair.first].insert(make_pair(ix,pair.second));
            }
        }
    }
#ifdef DEBUG
    cout << "tf .. done... " << endl;
#endif
    // 第二遍 遍历 : 算 inverse document frequency;
    // IDF = log( (文档总数)/(1+包含该词的文档数目) )
    // 文档总数 ： doc_vec.size()
    // 包含该词的文档数目 : _index_hash_map 中 map.size()
    const size_t DOC_NUM = doc_vec.size();
    assert(DOC_NUM>0);
    for(auto& word_indexmap:_index_hash_map){
        // 求 TF * IDF
        double doc_occurence = static_cast<double>(word_indexmap.second.size());
        double idf = static_cast<double>(log( static_cast<double>(DOC_NUM) / (1+doc_occurence)));
#ifdef DEBUG
        //  idf has the chance to be negtive , see << the beauty of math>> ;
        if(idf < 0){
            cout<< " ---------------------- " << DOC_NUM/(1+doc_occurence) << " " << DOC_NUM << " " << 1+doc_occurence << endl;
        }
#endif
        // 求出idf 后 和 tf 相乘 得出  未归一化的  权重 
        map<size_t,double>  index_weight_map;
        for(map<size_t,double>::iterator iter = word_indexmap.second.begin(); iter!=word_indexmap.second.end();++iter){
            size_t index = iter->first;
            double weight = static_cast<double>((iter->second) * idf);
            if(weight > 0){
                index_weight_map.insert(make_pair(index,weight));
            }else{
#ifdef DEBUG
            cout << " weight " << weight << " doc index " << index <<endl;
#endif
            }
        }
        _index_hash_map_idf.insert(make_pair(word_indexmap.first,index_weight_map)); 
    }
#ifdef DEBUG
    cout << " idf * tf ...done .. ." << endl;
#endif
    // 第三遍遍历 对权重进行归一化
    //  先计算每篇文章的权重和 存起来
    
    map<size_t,double> docid_weight_map; // docid,weight
    for(auto& word_indexset : _index_hash_map_idf){
        for(auto& id_weight : word_indexset.second){
             docid_weight_map[id_weight.first] += id_weight.second;
        }
    }
#ifdef DEBUG
    cout << " weight sum  ... done ... " << endl;
#endif
    // 再把_index_hash_map中的权重值归一化
    for(auto& word_indexmap : _index_hash_map_idf){
        map<size_t,double> temp_map;
        for(auto& id_weight : word_indexmap.second){
            double sum_weight = docid_weight_map.find(id_weight.first)->second;
            assert(sum_weight>0);
            double weight =  static_cast<double>(id_weight.second / sum_weight);
            temp_map.insert(make_pair(id_weight.first,weight));
        }
        _index_hash_map_weight.insert(make_pair(word_indexmap.first,temp_map));
    }
#ifdef DEBUG
    cout << "  ... done ... " << endl;
#endif
}

void write_index_to_file(){
    Configure* pconf = Configure::getInstance();
    string doc_index_path = pconf->getConfigByName("DOC_INDEX_PATH");
    ofstream index_writer(doc_index_path.c_str());
    if(!index_writer.is_open()){
        throw runtime_error("打不开倒排文件");
    }
    for(auto& pair : _index_hash_map_weight){
        index_writer << pair.first << " ";
        for(auto& idxpr:pair.second){
            index_writer << idxpr.first << " " << idxpr.second << " " ;   
        }
        index_writer << std::endl;
    }
    index_writer.close();
}

int main(){
    // 生成文档倒排索引
    // 首先读取文档库到 doc_vec中
#ifdef DEBUG
    cout << " building_document_vec ..." << endl;
#endif
    build_document_vec();
    // 准备切词库
    string jieba_dict_path,hmm_dict_path;
    Configure *pconf = Configure::getInstance();
    hmm_dict_path = pconf->getConfigByName("HMM_DICT_FILE");
    jieba_dict_path = pconf->getConfigByName("JIEBA_DICT_FILE");
    CppJieba::MixSegment seg(jieba_dict_path,hmm_dict_path);
#ifdef DEBUG
    cout << "building  inverse index... " << endl;
#endif
    // 建立倒排索引
    build_inverse_index(seg);
    // 把索引写入索引文件
#ifdef DEBUG
    cout << "write index to file... " << endl;
#endif
    write_index_to_file();
    return EXIT_SUCCESS;
}

