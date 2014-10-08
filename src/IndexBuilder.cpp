/*************************************************************************
	> File Name: IndexBuilder.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年10月08日 星期三 15时39分55秒
 ************************************************************************/
// 建立倒排索引

#include "../include/Diction.h"
#include "../include/Configure.h"
#include "../include/Log.hpp"

#include <map>
#include <stdlib.h>
#include <string>
#include <strings.h>
#include <string.h>
#include <set>
#include <utility>

using namespace std;
//定义数据结构 hashmap(词，在词典中的下标);
typedef map<string,set<size_t> > MAP_TYPE;

MAP_TYPE index_map;

void splitIntoLetters(const string& key_word,vector<string>& letters_vec){
    //这个函数要求输入的是utf8编码
    const string::size_type len = key_word.size();
    char *arr = new char[len+1];
    bzero(arr,len+1);
    strncpy(arr,key_word.c_str(),len);
    for(string::size_type ix = 0;ix!=len;){
        if(arr[ix]<0){
            //若是utf编码（第一个字节是负数),3个字节表示一个utf8汉字
            char* chinese_letter = new char[4];
            chinese_letter[0] = arr[ix++];
            chinese_letter[1] = arr[ix++];
            chinese_letter[2] = arr[ix++];
            chinese_letter[3] = 0;
            string letter_str(chinese_letter);
            delete[] chinese_letter;
            letters_vec.push_back(letter_str);
        }else if(arr[ix]!=0){
            // 是asiII 字符
            char *letter= new char[2];
            letter[0] = arr[ix++];
            letter[1] = 0;
            string s(letter);
            letters_vec.push_back(s);
            delete[] letter;
        }else{
            //空格不要
            ++ix;
        }
    }
    delete[] arr;
}

bool saveIndex(){
    Configure* pconf = Configure::getInstance();
    string index_path = pconf->getConfigByName("INDEX_PATH");
    ofstream ofs(index_path.c_str());
    if(!ofs.is_open()){
        _LogError("索引文件打开失败");
        return false;
    }
    // 遍历map 
    for(MAP_TYPE::iterator iter = index_map.begin();iter!=index_map.end();++iter){
        ofs << iter->first << " ";
        for(set<size_t>::iterator indexes = (iter->second).begin();indexes != (iter->second).end();++indexes){
            ofs << *indexes << " ";
        }
        ofs << endl;
    }

    ofs.close();
    return true;
}

int main(){
    // 加载词典
    Diction* pdict = Diction::getInstance();
    Diction::_DICT_VEC_TYPE& diction_vec = pdict->getDictionVec();
    // 把词典中的信息存入索引文件中 (把词典的每个关键词拆分成单个汉字和英文单词为单位，把下标存入当前字的set中)
#ifdef DEBUG
    cout << "diction size :" <<  diction_vec.size() << endl;
#endif
    for(Diction::_DICT_VEC_TYPE::size_type ix = 0 ; ix != diction_vec.size();++ix){
        string key_word = diction_vec[ix].first;
        //把当前的key_word 切分成 字为单位 存入vector中
        vector<string> letters_vec;
        letters_vec.clear();
        splitIntoLetters(key_word,letters_vec);
        // 将letters_vec 中的 每个字存入索引hashmap中
        for(auto letter: letters_vec){
            index_map[letter].insert(ix);
        }
#ifdef DEBUG
        cout << ix << endl;
#endif
    }
#ifdef DEBUG
    cout << "index map size : " << index_map.size() << endl;
#endif
    // 把索引存入索引文件
    if(!saveIndex()){
        _LogFatal("存索引失败");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
