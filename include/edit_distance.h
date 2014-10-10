/*************************************************************************
  > File Name: edit_distance.h
  > Author: HunkAnn
  > Mail: hunkann@gmail.com 453775948@qq.com 
  > Created Time: 2014年10月09日 星期四 23时47分29秒
 ************************************************************************/
#ifndef _EDIT_DISTANCE_H_
#define _EDIT_DISTANCE_H_

#include <string>
#include <stdlib.h> // mblen
#include <vector>
#include <string.h> // memset
#include <stdint.h> // uint32_t
#include <stdio.h> // perror


#define _ILEAGAL -1
#define _IS_ASCIII 1
#define _IS_GBK 2
#define _IS_UTF8 3


/*
   GB18030 
   第1位：0x81 ~ 0xFE 1000 0001 ~ 1111 1110 
   第2位：0x40 ~ 0x7E 0100 0000 ~ 0111 1110 
   或 者：0x80 ~ 0xFE 1000 0000 ~ 1111 1110 
   UTF-8 
   第1位：0xE0 ~ 0xEF 1110 0000 ~ 1110 1111 
   第2位：0x80 ~ 0xBF 1000 0000 ~ 1011 1111 
   第3位：0x80 ~ 0xBF 1000 0000 ~ 1011 1111 **/

/*
   1. 如果第1位是0就不需要判断的，一定是ASCII字符。 
   2. 如果第1位是1开头的，第2位是0开头的，一定是GB编码 编码。 
   3. 如果第1位是非1110开头的，则一定是GB 
*/
int get_character_code_type(const char* s) 
{ 
    for(int i = 0; s[i] != 0; ++i) 
    { 
        if(!(s[i] & 0x80))  // 0x80  1000 000
        { 
            // 如果第一位不是1是0,说明是ascii字符
            return 1;
        } 
        else if(!( (s[i] & 0xF0) ^ 0xE0)) 
        { 
            // 0xF0 11110000 与完--> xxx0 0000  
            // 0xE0 11100000
            //     ^xxx00000
            //     ---------
            //         00000 // 相同取0相异取1
            //     前3位如果是111那么异或11100000的结果就是00000000
            return 3; 
        } 
        else 
        { 
            return 2; 
        } 
    } 
    return -1; 
} 



bool str_into_vector(const std::string& str,std::vector<uint32_t>& vec){
    // 把字符串的单个汉字存入vector中
    vec.clear();
    for(std::string::size_type ix = 0;ix!=str.size();){
        // 判断编码类型
        int iret = get_character_code_type((char*)&str[ix]);
        if(iret == _ILEAGAL){
            perror("get_character_code_type illgal");
            return false;
        }
        else if(iret == _IS_ASCIII){
            // 是英文字符（单字节）
            vec.push_back((unsigned char)str[ix++]);
        }
        else if(_IS_GBK== iret){
            uint8_t high_byte = str[ix++];
            uint8_t low_byte = str[ix++];
            uint32_t whole = (high_byte<<8)+low_byte;
            vec.push_back(whole);
        }
        else if(_IS_UTF8 == iret){
            uint8_t _1st_byte = str[ix++];
            uint8_t _2nd_byte = str[ix++];
            uint8_t _3rd_byte = str[ix++];
            uint32_t _whole = (_1st_byte<<16)+(_2nd_byte<<8)+_3rd_byte;
            vec.push_back(_whole);
        }
        //delete[] buf;
    }
    return true;
}


uint32_t getMin(uint32_t a,uint32_t b,uint32_t c){
    uint32_t mid = (a<b)?a:b;
    return (mid<c)?mid:c;
}


int editDistance(std::vector<uint32_t>& sa,std::vector<uint32_t>& sb){
    int **matrix = new int *[sa.size() + 1];
    for (std::vector<uint32_t>::size_type i = 0; i <= sa.size(); ++i)
    {
        matrix[i] = new int[sb.size() + 1];
        memset(matrix[i], 0, sb.size() + 1);
    }

    for (std::vector<uint32_t>::size_type i = 0; i <= sa.size(); ++i)
    {
        matrix[i][0] = i;
    }
    for (std::vector<uint32_t>::size_type i = 0; i <= sb.size(); ++i)
    {
        matrix[0][i] = i;
    }

    for (std::vector<uint32_t>::size_type i = 1; i <= sa.size(); ++i)
    {
        for (std::vector<uint32_t>::size_type j = 1; j <= sb.size(); ++j)
        {

            int cost = sa[i - 1] == sb[j - 1] ? 0 : 1;
            matrix[i][j] =
                getMin( matrix[i - 1][j] + 1,
                        matrix[i][j - 1] + 1,
                        matrix[i - 1][j - 1] + cost);
        }
    }
    int ret = matrix[sa.size()][sb.size()];
    for (std::vector<uint32_t>::size_type i = 0; i <= sa.size(); ++i)
    {
        delete[] matrix[i];
    }
    delete matrix;
    return ret;
}



int getEditDistance(const std::string& candidite,const std::string& keyword){
    int distance = 0x11111111;
    if(candidite.size()&&keyword.size()){
        std::vector<uint32_t> va,vb;
        if(str_into_vector(keyword,va)){
            if(str_into_vector(candidite,vb)){
                distance = editDistance(va,vb);
            }
        }
    }
    return distance;
}




#endif
