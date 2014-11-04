/*************************************************************************
  > File Name: Query.cpp
  > Author: HunkAnn
  > Mail: hunkann@gmail.com 453775948@qq.com 
  > Created Time: Wed 22 Oct 2014 09:06:26 AM CST
 ************************************************************************/

#include "../include/Log.hpp"
#include "../include/Query.h"
#include "../include/CppJieba/HMMSegment.hpp"
#include "../include/CppJieba/MixSegment.hpp"
#include "../include/Configure.h"
#include <algorithm>
#include "../include/Diction.h"
#include "../include/json/json.h"

using namespace std;

vector<Document> _doc_vec;
vector<pair<int,int> > _offset_vec;

Query* Query::_s_p_instance = NULL;
MutexLock* Query::_s_lock = new MutexLock();
DocIndex* Query::_inverted_index = NULL;
// 把输入的字符串分词 并统计词频
Configure* pconf = Configure::getInstance();
string hmm_dict_path = pconf->getConfigByName("HMM_DICT_FILE");
string jieba_dict_path = pconf->getConfigByName("JIEBA_DICT_FILE");
CppJieba::MixSegment seg(jieba_dict_path,hmm_dict_path); 

void parse_query_string(const string& query_str,unordered_map<string,double>& query_wd_wgt_map){
    vector<string> word_vec;
    // 切词
    seg.cut(query_str,word_vec);
    // 统计词频
    map<string,int> fq_map;
    for(auto& word:word_vec){
       ++fq_map[word];
    }
    // 存储结果到输出参数
    vector<string>::size_type size = word_vec.size();
    for(auto& wd_fr:fq_map){
       query_wd_wgt_map.insert(make_pair(wd_fr.first,static_cast<double>((double)wd_fr.second/(double)size)));
    }
}

void get_inverted_map(unordered_map<string,double>& query_words_udmap,
        DocIndex* p_index,
        unordered_map<string,unordered_map<size_t,double> >& result_map,
        set<size_t>& docids){
    // 在倒排索引中取出所有查询词的倒排
    for(auto& word_frequency : query_words_udmap){
        unordered_map<size_t,double> word_index_temp;
        bool flag = p_index->getIndexes(word_frequency.first,word_index_temp);
        if(!flag){
            _LogError("找不到该单词的倒排表");
            return ;
        }
        for(auto& pair : word_index_temp ){
            result_map[word_frequency.first].insert(pair);
            docids.insert(pair.first);
        }
    }
}

void get_doc_vec(
        unordered_map<std::size_t, std::unordered_map<std::string, double> >& doc_vec_map,
        unordered_map<std::string, std::unordered_map<std::size_t, double> >& inverted_map, // TODO 倒排表不对
        set<size_t>& docids){
    // docid  --> word(查询词),fre 
    for(auto & word_idfre : inverted_map){
        for(auto& id_fre : word_idfre.second){
            if(docids.count(id_fre.first)!=0){
                // id在并集中
                doc_vec_map[id_fre.first].insert(make_pair(word_idfre.first,id_fre.second));
            }
        }
    }
}

void get_query_words_vec(unordered_map<string,double>& query_words_udmap,unordered_map<string,double>& word_weight,size_t doc_num){
    float sum = 0.0f;
    Diction* pdict = Diction::getInstance();
    Diction::_DICT_MAP_TYPE& dict = pdict->getDictionMap();
    for (auto& word_frequency : query_words_udmap)
    {
        double tf = word_frequency.second;
        double df = dict[word_frequency.first];
        double idf = (double)log2((double)doc_num / (1+df));
        double weight = (double)tf * idf;
        word_weight.insert(make_pair(word_frequency.first, weight));
        sum += weight; 
    }
    // 归一化
    for (auto &x : word_weight)
    {
        x.second = x.second / sum;
    }
}

void compute_similarity(unordered_map<string,double>& word_weight_map, // 查询向量
        unordered_map<size_t,unordered_map<string,double> > &doc_vec_map,// 文档向量
        multimap<double,size_t>& docid_map){ // 结果
    for (auto& id_wordweight : doc_vec_map){
        double ret = 0.0f;
        for (auto& word_weight : word_weight_map){
            ret += word_weight.second * id_wordweight.second[word_weight.first];
        }
        docid_map.insert(make_pair(ret, id_wordweight.first));
    }
}

string docs_to_json(vector<Document>& docs){
    Json::Value root;
    Json::Value arr;
    int cnt = 0;
    for (auto x : docs)
    {
        Json::Value elem;
        elem["title"] = x._title;
        elem["summary"] = x._content;
        arr.append(elem);
    }
    root["docs"] = arr;
    Json::FastWriter writer;
    Json::StyledWriter stlwriter;

    return stlwriter.write(root);	
} 

void read_doc(multimap<double,size_t>& docid_map // 存储　相似度　
        ,vector<Document>& docs){
    for(multimap<double,size_t>::reverse_iterator iter = docid_map.rbegin();iter!=docid_map.rend();++iter){
        // 取出文档
        docs.push_back(_doc_vec[iter->second]);
#ifdef DEBUG
        break;
#endif
    }
}

std::string Query::query(const std::string & query_str)
{
    //对查询字符串分词并统计词频
    std::unordered_map<std::string, double>  query_words_udmap;
    parse_query_string(query_str, query_words_udmap);
    // 在倒排索引中取出 每个词 的索引项
    std::unordered_map<std::string, std::unordered_map<std::size_t, double> > inverted_map;
    set<size_t> docids;
    get_inverted_map(query_words_udmap,_inverted_index,inverted_map,docids);
    //获取倒排表交集中文档的特征向量
    // docid --> word,weight word,weight ... 
    std::unordered_map<std::size_t, std::unordered_map<std::string, double> > doc_vec_map;
    get_doc_vec(doc_vec_map,inverted_map,docids);
    //获取查询词的特征向量
    std::unordered_map<std::string, double> word_weight;
    get_query_words_vec(query_words_udmap, word_weight,_doc_vec.size());
    //计算查询词和筛选出的每篇文档的相关性
    std::multimap<double, std::size_t> docid_map;
    compute_similarity(word_weight, doc_vec_map, docid_map);
    //根据最终排序结果，读取文档
    std::vector<Document> docs;
    read_doc(docid_map, docs);
#ifdef DEBUG
    cout << " 查出来了"<<  docs.size()<< "篇文章" << endl;
#endif
    string ret = docs_to_json(docs);
    return ret;
}


bool build_offsets_vec(vector<pair<int,int> >& _offset_vec){
    Configure* pconf = Configure::getInstance();
    string offset_path = pconf->getConfigByName("NEW_RIPE_LIB_OFFSET_PATH"); ifstream offset_reader; offset_reader.open(offset_path.c_str());
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
        _offset_vec.push_back(make_pair(offset,length));
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

bool build_document_vec(vector<Document>& _doc_vec,vector<pair<int,int> >& _offset_vec){
    Configure* pconf = Configure::getInstance();
    string ripe_path = pconf->getConfigByName("NEW_RIPE_LIB_PATH");
    ifstream ripe_reader;
    ripe_reader.open(ripe_path.c_str());
    if(!ripe_reader.is_open() ){
        _LogError("open %s failed",ripe_path.c_str());
        return false;
    }
    if(!build_offsets_vec(_offset_vec)){
        _LogError("build Offset vec error");
        return false;
    }
    // 读取偏移量vector 找出每篇文章的起始位置
    // 从RipePageLib中读解析出所有的Document 存入vector
    for(auto& pair : _offset_vec){
        ripe_reader.seekg(pair.first,ripe_reader.beg);   
        char *buf = new char[pair.second+1];
        bzero(buf,pair.second+1);
        ripe_reader.read(buf,pair.second);
        string doc_str(buf);
        _doc_vec.push_back(parseStr2Doc(doc_str));
        delete[] buf;
    }
    ripe_reader.close();
    return true;
}

Query::Query(){
    // 加载倒排表
    _inverted_index = DocIndex::getInstance();
    build_document_vec(_doc_vec,_offset_vec);
}

Query* Query::getInstance(){
    if(_s_p_instance == NULL){
        LockSafeGuard raii(*_s_lock);
        if(_s_p_instance == NULL){
            _s_p_instance = new Query();
        }
    }
}
