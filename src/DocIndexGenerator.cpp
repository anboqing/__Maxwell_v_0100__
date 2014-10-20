/*************************************************************************
  > File Name: DocIndexGenerator.cpp
  > Author: HunkAnn
  > Mail: hunkann@gmail.com 453775948@qq.com 
  > Created Time: Mon 20 Oct 2014 04:45:39 PM CST
 ************************************************************************/

#include<iostream>

using namespace std;

#include "../include/CleanGBKToken.hpp"
#include "../include/Log.hpp"
#include "../include/Configure.h"
#include "../include/Document.h"
#include "../include/CppJieba/HMMSegment.hpp"
#include "../include/CppJieba/MixSegment.hpp"

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

bool getTermFrequency(Document& doc,map<string,float>& doc_map,CppJieba::MixSegment& seg){
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
            pair.second /= size;
        }
        return true;
    }
    return false;
}

unordered_map<string,set<pair<size_t,float> > > _index_hash_map;

void build_inverse_index(CppJieba::MixSegment &seg){
    // 建立倒排索引 
    for(vector<Document>::size_type ix = 0 ; ix != doc_vec.size() ; ++ ix){
        map<string,float> doc_map;
        // 对每篇文章 ： 求每个词的tf
        if(getTermFrequency(doc_vec[ix],doc_map,seg)){
            // 然后把本文章的map 合并到_index_hash_map中去
            for(auto& pair : doc_map){
                _index_hash_map[pair.first].insert(make_pair(ix,pair.second));
            }
        }
    }
}

void write_index_to_file(){
    Configure* pconf = Configure::getInstance();
    string doc_index_path = pconf->getConfigByName("DOC_INDEX_PATH");
    ofstream index_writer(doc_index_path.c_str());
    if(!index_writer.is_open()){
        throw runtime_error("打不开倒排文件");
    }
    for(auto& pair : _index_hash_map){
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

