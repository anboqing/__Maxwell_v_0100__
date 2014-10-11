/*************************************************************************
> File Name: DictionBuilder.cpp
> Author: HunkAnn
> Mail: hunkann@gmail.com 453775948@qq.com 
> Created Time: 2014年10月06日 星期一 21时16分04秒
************************************************************************/

#include <vector>
#include <map>
#include <set>
#include <string>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdexcept>
#include <iostream>

#include "../include/CleanGBKToken.hpp"
#include "../include/Log.hpp"
#include "../include/Configure.h"
#include <string.h>
#include <errno.h>

#include "../include/CppJieba/HMMSegment.hpp"
#include "../include/CppJieba/MixSegment.hpp"
#include "../include/EncodingConverter.hpp"

using namespace std;

map<string,size_t> _dict_map;
set<string> _exclude_set;

void buildExcludeSet(){
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

void readFileToMap(const char* filename,CppJieba::MixSegment &seg){
    // 打开文件
    ifstream ifs(filename);
    if(!ifs.is_open()){
        _LogError("open %s file failed",filename);
        throw runtime_error("can not open file");
    }
    //把文件按行读取进来
    string words;
    vector<string> vec_line_words;
    while(words.clear(),getline(ifs,words)){ 
        // 清理标点符号
        cleanGBKToken(words);
        // 用格式转换器把gbk转换成utf8
        string new_words;
        if(!gbk_utf8(words,new_words)){
            _LogInfo("////%s",filename);
            continue;
        }
        vec_line_words.clear();
        // 用切词器切词
        seg.cut(new_words,vec_line_words);
        // 将本行的单词存入 _dict_map
        for(vector<string>::iterator iter = vec_line_words.begin();iter!=vec_line_words.end();++iter){
            if(!_exclude_set.count(*iter)){
                // 统计词频
                ++_dict_map[*iter];
            }
        }
    }
    ifs.close();
}

void traverseDir(const char *row_path,CppJieba::MixSegment& seg)
{
    DIR *dp = opendir(row_path);
    if (NULL == dp)
    {
        _LogError("open dir error : %s ",strerror(errno));
        throw std::runtime_error("cannot open directory");
    }
    chdir(row_path);
    struct dirent *entry;
    struct stat statbuf;

    while ((entry = readdir(dp)) != NULL)
    {
        stat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode))
        {
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                continue;
            traverseDir(entry->d_name,seg);
        }
        else
        {
            readFileToMap(entry->d_name,seg);
        }
    }
    chdir("..");
    closedir(dp);
}

int main(){
    // 构建单词排除集
    buildExcludeSet();
    // 1.遍历目录并且把单词和词频存入map
    string jieba_dict_path,hmm_dict_path;
    Configure *pconf = Configure::getInstance();
    hmm_dict_path = pconf->getConfigByName("HMM_DICT_FILE");
    jieba_dict_path = pconf->getConfigByName("JIEBA_DICT_FILE");
    CppJieba::MixSegment seg(jieba_dict_path,hmm_dict_path);
#ifdef DEBUG
    cout << " diction load finish " << endl;
#endif
    string raw_path = pconf->getConfigByName("RAW_PATH");
    traverseDir(raw_path.c_str(),seg);
    // 2.把map持久化到文件中
    string my_dict_path = pconf->getConfigByName("MY_DICT_PATH");
    ofstream ofs(my_dict_path.c_str());
    if(!ofs.is_open()){
        _LogFatal("cannot open %s",my_dict_path.c_str());
        throw runtime_error("cannot open mydict.dat");
    }
    //这行编译要用c++11
    for(auto p : _dict_map){
        ofs << p.first <<"  "<< p.second << "\n";
    }
    ofs.close();
}
