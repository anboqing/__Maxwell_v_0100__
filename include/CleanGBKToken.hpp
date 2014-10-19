#ifndef _CLEAN_GBK_TOKEN_H_
#define _CLEAN_GBK_TOKEN_H_

#include <stdlib.h>
#include <string>
#include <locale.h>

bool cleanGBKToken(std::string& in){
    if(in.size()==0){
        return false;
    }
    //注意输入的必须是gbk的编码
    size_t index = 0;
    size_t count = 0;
    unsigned char high_bytes,low_bytes;
    //On  startup  of  the  main  program, the portable "C" locale is selected as default. A program may be made portable to all locales by calling:
    setlocale(LC_ALL,"");
    while(index < in.size()-2){ //这里减1的原因是因为有双字节字符
        // mblen 返回下一个多字节字符的字节数
        count = mblen(&in[index],2);
        if(count == 1){
            //如果进入这里说明下一个不是多字节字符而是单子节字符（英文字符)
            //那么就要去掉英文标点
            if(isalpha(in[index])){
                //如果是字母就留着
                ++index;
            }else{
                //是否则就换成空格
                //in[index++] = ' ';
                in.replace(index++,1," ");
            }
        }else{
            low_bytes = static_cast<unsigned char>(in[index]);
            high_bytes = static_cast<unsigned char>(in[index+1]);
            // 如果在标点的范围就把它删掉
			if (((low_bytes>=0xa1&&low_bytes<=0xa9)&&(high_bytes>=0xa1&&high_bytes<=0xfe))||
				((low_bytes>=0xa8&&low_bytes<=0xa9)&&(high_bytes>=0x40&&high_bytes<=0xa0))){
				in.erase(index,2);
                //in.insert(index," ");
            }else
				index++;
        }
    }
    return true;
}

bool clean_UTF8_Token(std::string& in){
    if(in.size()==0){
        return false;
    }
    //注意输入的必须是UTF8的编码
    size_t index = 0;
    size_t count = 0;
    unsigned char beg_bytes,mid_bytes,end_bytes;
    //On  startup  of  the  main  program, the portable "C" locale is selected as default. A program may be made portable to all locales by calling:
    setlocale(LC_ALL,"");
    while(index < in.size()-2){ //这里减1的原因是因为有三字节字符
        // mblen 返回下一个多字节字符的字节数
        count = mblen(&in[index],2);
        if(count == 1){
            //如果进入这里说明下一个不是多字节字符而是单子节字符（英文字符)
            //那么就要去掉英文标点
            if(isalpha(in[index])){
                //如果是字母就留着
                ++index;
            }else{
                //是否则就换成空格
                //in[index++] = ' ';
                in.replace(index++,1," ");
            }
        }else{
            beg_bytes = static_cast<unsigned char>(in[index]);
            mid_bytes = static_cast<unsigned char>(in[index+1]);
            end_bytes = static_cast<unsigned char>(in[index+2]);
            // 如果在标点的范围就把它删掉 0xEA84BF~0xEAA980
            if (
                beg_bytes==0xEA && (mid_bytes > 0x84 && mid_bytes < 0xA9)
                &&( end_bytes > 0xBF && end_bytes < 0x80 )
               ){
				in.erase(index,2);
            }else
				index++;
        }
    }
    return true;
}
#endif
