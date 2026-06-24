#include <string>
#include <string.h>

#include "iconv.h"
#include "libcharset.h"
#include "localcharset.h"
#include "font_engine.h"

//エンコーディング変換。source_charset は変換元エンコーディング、to_charset は変換先エンコーディングです  
int32_t code_convert(const char *source_charset, const char *to_charset, const std::string& sourceStr, char *outData, int32_t outLen) //sourceStr は変換元エンコーディングの文字列です  
{  
    iconv_t cd = iconv_open(to_charset, source_charset);//変換ハンドルを取得します。void 型です  
    if (cd == 0)  
        return -1;  
  
    char *inbuf = (char *)sourceStr.c_str();
    size_t inlen = sourceStr.size();
	char *pouData = outData;	// このポインタを追加しているのは、iconv 関数内部で outbuf の値が変更されるためです。渡された値がヒープアドレスの場合、元のアドレスを解放できずメモリリークの原因になります
    size_t outlen = outLen;		// 上記と同様に、iconv 関数は outbytesLeft の値も変更します。
	
    memset(pouData, 0, outlen);  
    if (iconv(cd, &inbuf, &inlen, &pouData, &outlen) == -1){
        iconv_close(cd);
        return -1;
    }

    iconv_close(cd);
    return outLen-outlen;
}  

//gbk转UTF-8    
int32_t gbk_to_utf8(const char *gbkStr, char *utf8Str, int32_t bufLen)// 渡される strGbk は GBK エンコーディングです   
{
	if( (NULL == gbkStr) || (NULL == utf8Str) )
		return 0;
	
	std::string inStr = gbkStr;
	return code_convert("gb2312", "utf-8", inStr, utf8Str, bufLen);
}

//gbk转unicode
int32_t gbk_to_unicode(const char *gbkStr, char *unicodeStr, int32_t bufLen)// 渡される strGbk は GBK エンコーディングです   
{
	if( (NULL == gbkStr) || (NULL == unicodeStr) )
		return 0;
	
	std::string inStr = gbkStr;
	return code_convert("gb2312", "UCS-2LE", inStr, unicodeStr, bufLen);
}


// UTF-8 文字列長をカウントします
static bool bIsUTF8Start(uint8_t c)
{	
	// 最上位ビットが 0
	if(0 == (c>>7)){
		return true;
	// 上位 2 ビットが 11（二進数）
	}else if(0xc0 == (c&0xc0)){
		return true;
	}else{
		return false;
	}
}
int32_t utf8_strlen(const char *utf8Str)
{
    int i = 0, j = 0;
    while (utf8Str[i]) {
        if (bIsUTF8Start(utf8Str[i])) {
            j++;
        } 
        i++;
    }
    return j;
}

//UTF-8转gbk
int32_t utf8_to_gbk(const char *utf8Str, char *gbkStr, int32_t bufLen)  
{  
	if( (NULL == utf8Str) || (NULL == gbkStr) )
		return 0;
	
	std::string inStr = utf8Str;
	return code_convert("utf-8", "gb2312", inStr, gbkStr, bufLen);
}

//UTF-8转unicode
int32_t utf8_to_unicode(const char *utf8Str, char *uniCodeStr, int32_t uniCodeLen)  
{  
	if( (NULL == utf8Str) || (NULL == uniCodeStr) )
		return 0;
	
	std::string inStr = utf8Str;
	return code_convert("utf-8", "UCS-2LE", inStr, uniCodeStr, uniCodeLen);
}

