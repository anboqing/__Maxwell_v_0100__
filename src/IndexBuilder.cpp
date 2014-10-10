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
#include "../include/string_tools.h"
using namespace std;
//定义数据结构 hashmap(词，在词典中的下标);
typedef map<string,set<size_t> > MAP_TYPE;

MAP_TYPE index_map;

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
