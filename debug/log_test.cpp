/*************************************************************************
	> File Name: log_test.cpp
	> Author: HunkAnn
	> Mail: hunkann@gmail.com 453775948@qq.com 
	> Created Time: 2014年09月29日 星期一 00时21分06秒
 ************************************************************************/
#include "Log.hpp"

int main(){
    LogDebug("%s","debug");
    LogInfo("%s","info");
    LogWarn("%s","warn");
    LogError("%s","error");
    LogFatal("%s,%s","fatal","----");
}
