/*************************************************************************
  > File Name: RipeLib_Distinctor.cpp
  > Author: HunkAnn
  > Mail: hunkann@gmail.com 453775948@qq.com 
  > Created Time: Wed 15 Oct 2014 11:57:08 PM CST
 ************************************************************************/

// g++ -o ../bin/RipeLib_Distinctor RipeLib_Distinctor.cpp Configure.cpp  -lpthread -I../include -std=c++11 -DLOGLEVER=LOG_ERROR 

#include "../include/Document.h"
#include "../include/Configure.h"
#include "../include/CppJieba/HMMSegment.hpp"
#include "../include/CppJieba/MixSegment.hpp"
#include "../include/EncodingConverter.hpp"
#include "../include/CleanGBKToken.hpp"
#include "../include/Log.hpp"

#include <sstream>
#include <vector>
#include <fstream>
#include <iterator> // istreambuf_iterator
#include <utility>
#include <iostream>
#include <queue>
#include <strings.h>

using namespace std;

set<int> redundant_set;
vector<Document> doc_vec;
vector<pair<int,int> > offset_vec; // 存放所有的偏移量
vector<vector<string> > fingerprint_vec;
set<string> _exclude_set;

bool build_offsets_vec(){
    Configure* pconf = Configure::getInstance();
    string offset_path = pconf->getConfigByName("RIPE_LIB_OFFSET_PATH"); ifstream offset_reader; offset_reader.open(offset_path.c_str());
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
// 把 <doc> <docid></docid><url></url><title></title><content></content></doc>中的内容解析出来
    string url,title,content;
    url = parseTag(doc_str,"<url>","</url>");
    title = parseTag(doc_str,"<title>","</title>");
    content = parseTag(doc_str,"<content>","</content>");
    return *(new Document(doc_id++,url,title,content));
}

bool build_document_vec(){
    Configure* pconf = Configure::getInstance();
    string ripe_path = pconf->getConfigByName("RIPE_LIB_PATH");
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

// 先提取每篇文章的词频最高的10个词
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

class Data{
    public:
        Data(const std::string& wd,int fr):_word(wd),_frequency(fr){}
        friend bool operator<(const Data& lo,const Data& ro){
            return lo._frequency < ro._frequency;
        }
        std::string _word;
        int _frequency;
};

bool getDocTopTen(Document& doc,CppJieba::MixSegment &seg){
    string content = doc._content;
    if( !(content.empty()) ){
        vector<string> vec_words;
        // 清理标点符号 （content 是utf8 编号) 这里得换成 清理 utf8 标点符号
        if(!clean_UTF8_Token(content)){
            cout <<" 清理标点符号错误 "<<  doc._url << endl;
        }
        vec_words.clear();
        // 用切词器切词
        seg.cut(content,vec_words);
        map<string,int> word_fre_map;
        for(vector<string>::iterator iter = vec_words.begin();iter!=vec_words.end();++iter){
            if(!_exclude_set.count(*iter)){
                // 统计词频
                ++word_fre_map[*iter];
            }
        }
        // big_root_heap 对词频排序
        priority_queue<Data> big_root_heap;
        for(auto& pair : word_fre_map){
            Data data(pair.first,pair.second);
            big_root_heap.push(data);
        }
        // 求出词频最高的10个词 , 存入 fingerPrint_vec<vector<topten> >
        vector<string> temp_vec;
        big_root_heap.pop();
        for(int count = 10 ; --count ;){
            temp_vec.push_back(big_root_heap.top()._word);
            big_root_heap.pop();
        }
        fingerprint_vec.push_back(temp_vec);
    }else{
        _LogError("content is empty : %d" , doc._docid );
        return false;
    }
    return true;
}

void extractTopn(CppJieba::MixSegment& seg){
    // 提取每篇文章词频最高的10个词
    for(auto& doc: doc_vec){
        if(!getDocTopTen(doc,seg)){
            redundant_set.insert(doc._docid);  
        }
    }
}

bool has_similiar_fp(long ix,long jx){
    vector<string>& va = fingerprint_vec[ix];
    vector<string>& vb = fingerprint_vec[jx];
    // 如何判断va和vb有多少个相同元素呢？
    // 方案 1 ： 依次两两比较 , 显然这办法太傻
    // 方案 2 ： 将所有元素放入一个集合里，集合的大小和va.size()+vb.size() 的差值，就是重复元素的个数（哈哈哈,bingle!!!!）
    //set<string> st(va.begin(),va.end());
    set<string> st;
    for(auto& word:va){
        st.insert(word);
    }
    for(auto& word:vb){
        st.insert(word);
    }
    // 如果有6个以上相同 ，就判定为相似
    if((va.size()+vb.size()-st.size()) >= 6){
        return true;
    }else{
        return false;
    }
}

// 该向量存储冗余的doc的id
void distinct_same_doc(){
    // 先2,2比较指纹，相同的就添加到冗余集合里
    for(vector<vector<string> >::size_type ix = 0 ;ix!=fingerprint_vec.size() ; ++ ix){
        if(redundant_set.count(ix)){
            continue;
        }
        for(vector<vector<string> >::size_type jx = 0; jx!=ix && jx!=fingerprint_vec.size();++jx){
            if( !(redundant_set.count(jx)) && has_similiar_fp(ix,jx)){
                redundant_set.insert(jx);
            }
        }
    }
#ifdef DEBUG
    cout << "redundant_set.size() : "<< redundant_set.size() << endl;
#endif
}

ofstream ofs,offset_writer;
static size_t docid = 1;

void write_doc_to_file(const string& url,const string& title,const string& content){
    // 将结果写入 文本库 并且记录偏移量以便 日后查找
    long begin = ofs.tellp();
    ofs << "<doc>" << endl << "<docid>" << docid++ << "</docid>"<< endl << "<url>" << url << "</url>"<<endl<< "<title>" << title<< "</title>"<<endl<< "<content>" << content<< "</content>"<< endl << "</doc>" <<endl;
    long end = ofs.tellp();
    // 写入偏移量  offset length  (偏移量 ，本篇文章的长度 )
    offset_writer << begin << " " << end-begin << endl;
}

void build_new_ripepagelib(){
    // 把冗余集合之外的文章重新写回到new_lib中
    for(vector<Document>::size_type ix = 0 ; ix != doc_vec.size() ; ++ ix){
        if(!(redundant_set.count(ix))){
            write_doc_to_file(doc_vec[ix]._url,doc_vec[ix]._title,doc_vec[ix]._content);
        }
    }
}

int main(){
    // 读取文章到 vector<Document> doc_vec 中
    build_document_vec();
#ifdef DEBUG
    cout << "build_document_vec complete ..."<< endl;
#endif
    // 读取停用词表
    build_exclude_set();
#ifdef DEBUG
    cout << "build_exclude_set complete ..."<< endl;
#endif
    // 准备切词库
    string jieba_dict_path,hmm_dict_path;
    Configure *pconf = Configure::getInstance();
    hmm_dict_path = pconf->getConfigByName("HMM_DICT_FILE");
    jieba_dict_path = pconf->getConfigByName("JIEBA_DICT_FILE");
    CppJieba::MixSegment seg(jieba_dict_path,hmm_dict_path);
    // 提取每篇文章的top10 words into vector<vector<string> > fingerprint_vec;
    extractTopn(seg);
#ifdef DEBUG
    cout << "extract top n complete ..."<< endl;
#endif
    // 比较每两篇文章的top10单词有多少相同，如果有7个以上相同就视为相似，就把该相似文章去掉
    distinct_same_doc();
#ifdef DEBUG
    cout << "distinct_same_doc complete ..."<< endl;
#endif
    string ripe_lib_path,offset_path;
    ripe_lib_path = pconf->getConfigByName("NEW_RIPE_LIB_PATH");
    offset_path = pconf->getConfigByName("NEW_RIPE_LIB_OFFSET_PATH");
    ofs.open(ripe_lib_path.c_str(),ios_base::trunc);
    if(!(ofs.is_open())){
        _LogFatal("文本库路径错误");
        exit(EXIT_FAILURE);
    }
    offset_writer.open(offset_path.c_str(),ios_base::trunc);
    if(!(offset_writer.is_open())){
        _LogFatal("文本库偏移文件路径错误");
        exit(EXIT_FAILURE);
    }
    build_new_ripepagelib();
    ofs.close();
    offset_writer.close();
    return EXIT_SUCCESS;
}
