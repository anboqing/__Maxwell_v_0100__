/*************************************************************************
	> File Name: ../src/RipeLib_Generator.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: Wed 15 Oct 2014 04:43:42 PM CST
 ************************************************************************/

// 本文件从原始文本目录中生成原始文件库

// g++ -o ../bin/RipeLib_Generator RipeLib_Generator.cpp Configure.cpp -lpthread -std=c++11 -I../include/

#include "../include/Log.hpp"
#include "../include/Configure.h"
#include "../include/EncodingConverter.hpp"

#include <stdlib.h>
#include <fstream>
#include <sys/types.h> // opendir stat
#include <dirent.h> // opendir
#include <unistd.h> // chdir stat
#include <sys/stat.h> // stat
#include <sstream>

using namespace std;

bool parseTitle(const string& text,string& title){
    // 从文本中解析出标题
    // 读取最多10行 ，查找 “ 标题 ” 二字 ，找到就把其后面的内容返回，否则就把 文档的第一句话返回
    int count = 10;
    istringstream sreader(text);
    string line;

    string gbk_biaoti ;
    utf8_gbk("【 标  题 】",gbk_biaoti);

    while( getline(sreader,line) && count--){
        string::size_type pos;
        if( (pos = line.find(gbk_biaoti)) != string::npos ){
            // 如果找到 [标 题]标签 把标签后面的 内容存起来
           title = line.substr(pos+gbk_biaoti.size()); 
           return true;
        }
    }
    // 如果没找到的话 就把文档的第一句返回
    if(title.empty()){
        sreader.seekg(ios_base::beg);
        getline(sreader,title);
    }
    return false;
}

string ripe_lib_path;
string offset_path;
string raw_path;
ofstream ofs,offset_writer;

void init(){
    // 执行一些初始化操作，避免多次执行，造成浪费
    Configure* pconf = Configure::getInstance();
    ripe_lib_path = pconf->getConfigByName("RIPE_LIB_PATH");
    offset_path = pconf->getConfigByName("RIPE_LIB_OFFSET_PATH");
    raw_path = pconf->getConfigByName("RAW_PATH");
    ofs.open(ripe_lib_path.c_str(),ios_base::app);
    if(!(ofs.is_open())){
        _LogFatal("文本库路径错误");
        exit(EXIT_FAILURE);
    }
    offset_writer.open(offset_path.c_str(),ios_base::ate);
    if(!(offset_writer.is_open())){
        _LogFatal("文本库偏移文件路径错误");
        exit(EXIT_FAILURE);
    }
}

static size_t docid = 1;
void write_doc_to_file(const string& url,const string& title,const string& content){
    // 将结果写入 文本库 并且记录偏移量以便 日后查找
    long begin = ofs.tellp();
    string content_utf8;
    gbk_utf8(content,content_utf8);
    string title_utf8;
    gbk_utf8(title,title_utf8);
    ofs << "<doc>" << endl << "<docid>" << docid++ << "</docid>"<< endl << "<url>" << url << "</url>"<<endl<< "<title>" << title_utf8 << "</title>"<<endl<< "<content>" << content_utf8 << "</content>"<< endl << "</doc>" <<endl;
    long end = ofs.tellp();
    // 写入偏移量  offset length  (偏移量 ，本篇文章的长度 )
    offset_writer << begin << " " << end-begin << endl;
}

void parseDocument(const string& file_path){
    // 把文本 解析成一个Document对象
    ifstream ifs(file_path.c_str());
    if(!ifs){
        // 如果打不开该文件，直接忽略，不要这个文件了
        return;
    }
    // 读取一整篇文件 下面不懂自行google,哈哈 欧巴就不告诉你死咪达
    istreambuf_iterator<char> begin(ifs),end;
    string text(begin,end);
    
    // 解析文档的各个部分
    string title,content;
    bool is_text_regular = parseTitle(text,title);
    //  The get_current_dir_name() function copies an absolute pathname of the current working directory
    string url(string(get_current_dir_name())+"/"+file_path);
    // 获取正文部分： 查找 正文 二字 ，找到就在后面
    if(is_text_regular){
        // 如果是 “ 正常” 文本 就 返回 正文 后面的 部分
        string gbk_zhengwen;
        utf8_gbk("【 正  文 】",gbk_zhengwen);
        content = text.substr(text.find(gbk_zhengwen)+gbk_zhengwen.size());
    }else{
        // 如果文档不正常 整个文档 看作content
        content = text;
    }
    // 把解析好的文档 写入 文本库
    write_doc_to_file(url,title,content);
}

void traverse_dir(const std::string & dir)
{
    DIR *pdir = opendir(dir.c_str());
    if (pdir == 0)
    {
        return;
    }
    chdir(dir.c_str());
    struct dirent *entry;
    struct stat statbuf;
    while ((entry = readdir(pdir)) != NULL)
    {
        std::string path = std::string(entry->d_name);
        stat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode))
        {
            if (path == (".") || path == (".."))
            {
                continue;
            }
            traverse_dir(path);
        }
        else
        {
            parseDocument(path);
        }
    }
    chdir("..");
    closedir(pdir);
}

int main(){
    init();
    traverse_dir(raw_path);
    ofs.close();
    offset_writer.close();
}
