/*************************************************************************
	> File Name: ../include/Document.h
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: Wed 15 Oct 2014 04:44:43 PM CST
 ************************************************************************/
#ifndef _DOCUMENT_H_
#define _DOCUMENT_H_

#include <string>
//这个类用来表示一个文档对象
class Document{
public:
    Document(size_t id,const std::string& url,
            const std::string& title,const std::string& content):
        _docid(id),_url(url),_title(title),_content(content){
        }
    /*size_t getDocId()const{*/
        //return _docid;
    //}
    //const std::string& getDocUrl()const{
        //return _url;
    //}
    //const std::string& getDocTitle()const{
        //return _title;
    //}
    //const std::string& getDocContent()const{
        //return _content;
    /*}*/
    size_t _docid;
    std::string _url;
    std::string _title;
    std::string _content;
};

#endif
