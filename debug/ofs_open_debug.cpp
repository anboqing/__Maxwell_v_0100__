/*************************************************************************
	> File Name: ofs_open_debug.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: Wed 15 Oct 2014 07:18:38 PM CST
 ************************************************************************/

#include<iostream>
#include <fstream>
#include "../include/Configure.h"

using namespace std;

string ripe_lib_path,offset_path,raw_path;
ofstream offset_writer,ofs;

/*void init(){*/
    //// 执行一些初始化操作，避免多次执行，造成浪费
    //Configure* pconf = Configure::getInstance();
    //ripe_lib_path = pconf->getConfigByName("RIPE_LIB_PATH");
    //offset_path = pconf->getConfigByName("RIPE_LIB_OFFSET_PATH");
    //raw_path = pconf->getConfigByName("RAW_PATH");
    //ofs.open(ripe_lib_path.c_str(),ios_base::app);
    //ofs << " hello " << endl;
    //if(!(ofs.is_open())){
        ////_LogFatal("文本库路径错误");
        //perror(ripe_lib_path.c_str());
        //exit(EXIT_FAILURE);
    //}
    //offset_writer.open(offset_path.c_str(),ios_base::ate);
    //if(!(offset_writer.is_open())){
        ////_LogFatal("文本库偏移文件路径错误");
        //perror(offset_path.c_str());
        //exit(EXIT_FAILURE);
    //}
//}

int main(){
    ofs.open("/home/anboqing__/Code/CASES/Maxwell/dict/ripepage_lib.dat",ios_base::app);
    if(!(ofs.is_open())){
        cout << " can not open file " << endl;
    }else{
        cout << " open file success! " << endl;
    }
    ofs << " hello " << endl;
    ofs.close();
}
