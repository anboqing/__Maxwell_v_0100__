//编码转换器 
#ifndef _ENCODING_CONVERTER_H_
#define _ENCODING_CONVERTER_H_

#include <iconv.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <stdio.h>
#include <string>

using namespace std;

int code_convert(char *from_charset,char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
    iconv_t cd;
    char **pin = &inbuf;
    char **pout = &outbuf;

    cd = iconv_open(to_charset,from_charset);
    if ( cd == (iconv_t)-1){
        return -1;
    }
    memset(outbuf,0,outlen);
    if (iconv(cd,pin,&inlen,pout,&outlen) ==(size_t)-1){
        //perror("iconv error : ");
        return -1;
    }
    iconv_close(cd);
    return 0;
}

bool utf8_gbk(string instr,string& outstr)
{
    size_t inlen  = strlen(instr.c_str());
    size_t outlen = inlen*2;
    char *inbuf = new char[inlen];
    strncpy(inbuf,instr.c_str(),inlen);
    char *outbuf = new char[outlen];
    char *res = outbuf;
    int iret = code_convert((char *)"UTF8",(char *)"GBK//IGNORE",inbuf,inlen,outbuf,outlen);
    if(iret < 0){
        return false;
    }
    outlen = strlen(res);
    outstr.resize(outlen);
    bzero(const_cast<char *>(outstr.c_str()),outlen);
    strncpy(const_cast<char*>(outstr.c_str()),res,outlen);
    delete[] inbuf;
    delete[] outbuf;
    return true;    
}

bool gbk_utf8(string instr,string& outstr)
{
    size_t inlen  = strlen(instr.c_str());
    size_t outlen = inlen*2;
    char *inbuf = new char[inlen];
    strncpy(inbuf,instr.c_str(),inlen);
    char *outbuf = new char[outlen];
    char *res = outbuf;
    int iret = code_convert((char *)"GBK",(char *)"UTF8//IGNORE",inbuf,inlen,outbuf,outlen);
    if(iret < 0){
        return false;
    }
    outlen = strlen(res);
    outstr.resize(outlen);
    bzero(const_cast<char *>(outstr.c_str()),outlen);
    strncpy(const_cast<char *>(outstr.c_str()),res,outlen);
    delete[] inbuf;
    delete[] outbuf;
    return true;    
}

#endif
