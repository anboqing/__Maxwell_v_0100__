/*************************************************************************
	> File Name: Uncopyable.h
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年09月27日 星期六 22时57分08秒
 ************************************************************************/

#ifndef _MAXWELL_UNCOPYABLE_H_
#define _MAXWELL_UNCOPYABLE_H_

class Uncopyable{
public:
    Uncopyable(){}
    ~Uncopyable(){}
private:
    //本类作用是让子类可以实现禁止复制
    Uncopyable(const Uncopyable &);
    Uncopyable &operator=(const Uncopyable&);
};

#endif
