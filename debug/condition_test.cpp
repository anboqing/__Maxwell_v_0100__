/*************************************************************************
  > File Name: condition_test.cpp
  > Author: HunkAnn
  > Mail: hunkann@gmail.com 453775948@qq.com 
  > Created Time: 2014年09月29日 星期一 18时58分37秒
 ************************************************************************/

#include "../include/Condition.h"
#include "../include/Lock.h"

#include <unistd.h> // sleep
#include <iostream>
#include <queue>

using namespace std;
//用一个队列模拟临界区
static queue<int> bow;
static MutexLock lock;
static Condition bow_empty_cond(lock);
static Condition bow_full_cond(lock);
static void* consumer_thread_func(void*);

int main(){
    //模拟一个消费者生产者来测试Condition类
    /*
     *有一个生产者在生产产品，这些产品将提供给若干个消费者去消费，为了使生产者和消费者能并发执行，在两者之间设置一个具有多个缓冲区的缓冲池，生产者将它生产的产品放入一个缓冲区中，消费者可以从缓冲区中取走产品进行消费，显然生产者和消费者之间必须保持同步，即不允许消费者到一个空的缓冲区中取产品，也不允许生产者向一个已经放入产品的缓冲区中再次投放产品。
     * */

    pthread_t tid;
    //用主线程模拟生产者 ， 用10个其他线程模拟消费者
    for(int ix = 0; ix !=3;++ix){
        pthread_create(&tid,NULL,consumer_thread_func,NULL);
    }
    //
    for(;;){
        //生产者（在队列空时)将它生产的产品放入一个缓冲区中
        lock.lock();
        while(bow.size()!=0){
            //队列不空就等待消费者消费完
            bow_empty_cond.wait();
        }
        //队列空就生产
        bow.push(1);
        bow.push(2);
        cout << pthread_self()%10 << " --------produce "<< bow.size() << " product" << endl;
        //通知等待消费的消费者
        lock.unlock();
        bow_full_cond.wakeUpAll();
        sleep(1);
    }
}

void *consumer_thread_func(void *arg){
    //消费者行为： 从队列中取出一个数，打印出来
    for(;;){
        lock.lock();
        while(bow.size()==0){
            //如果队列空就等待消费
            bow_full_cond.wait();
        }
        //如果队列不空就消费并唤醒生产者生产
        bow.pop();
        cout << pthread_self()%10 << " consume a product , queue size is : "<< bow.size()<< endl;
        if(bow.size()==0)
            bow_empty_cond.wakeUp();
        lock.unlock();
        sleep(1);
    }
    return arg;
}
