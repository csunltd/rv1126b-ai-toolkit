/**
 *
 * Copyright 2021 by Guangzhou Easy EAI Technologny Co.,Ltd.
 * website: www.easy-eai.com
 *
 * Author: Jiehao.Zhong <zhongjiehao@easy-eai.com>
 * 
 * this interface repackaged by the third-party open source library (libini), libini version: 1.1.10
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * License file for more details.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "libini.h"

//#include "ini_wrapper.h"

int32_t ini_section_exist(const char *file, const char *pSection)
{
    int32_t ret = -1;
    
    //ファイルが存在するか確認します
    if (access(file, F_OK) == -1)
        return ret;
    
    //ファイルを開きます
    ini_fd_t ini_file = ini_open(file, "r", ";");
    if (ini_file == 0){
        printf("ini_open %s error!,%s\n", file, strerror(errno));
        return ret;
    }
    
    ret = ini_locateHeading(ini_file, pSection);
    
    //ファイルを閉じます
    ini_close(ini_file);

    return ret;
}

int32_t ini_read_int(const char *file, const char *pSection, const char *pKey)
{
    int32_t value = -1;
    
    //ファイルが存在するか確認します
    if (access(file, F_OK) == -1)
        return value;
    
    //ファイルを開きます
    ini_fd_t ini_file = ini_open(file, "r", ";");
    if (ini_file == 0){
        printf("ini_open %s error! %s\n", file, strerror(errno));
        return value;
    }
    
    //設定情報を読み込みます
    ini_locateHeading(ini_file, pSection);
    ini_locateKey(ini_file, pKey);
    
    int res = ini_readInt(ini_file, &value);
    if(-1 == res){
        value = -1;
        printf("ini_readInt error! [%s]->%s not yet created\n", pSection, pKey);
    }
    
    //ファイルを閉じます
    ini_close(ini_file);
    
    return value;
}

// このインターフェースをマルチスレッドで使用する場合は、ロックを追加することを推奨します
const char *ini_read_string(const char *file, const char *pSection, const char *pKey)
{
    const char *retStr = NULL;
    static char strValue[2048] = {0};
    
    //ファイルが存在するか確認します
    if (access(file, F_OK) == -1)
        return retStr;
    
    //ファイルを開きます
    ini_fd_t ini_file = ini_open(file, "r", ";");
    if (ini_file == 0) {
        printf("ini_open %s error!,%s\n", file, strerror(errno));
        return retStr;
    }
    
    //設定情報を読み込みます
    ini_locateHeading(ini_file, pSection);
    ini_locateKey(ini_file, pKey);

    //ここでマルチスレッド呼び出しを行うことはできません。ロックに注意してください
    // ========================== lock ==========================
    bzero(strValue, sizeof(strValue));
    int res = ini_readString(ini_file, strValue, sizeof(strValue));
    if(-1 == res){
        printf("ini_readString error!,%s,[%s]->%s\n", strerror(errno), pSection, pKey);
    }else{
        retStr = strValue;
    }
    // ========================= unlock =========================
    
    //ファイルを閉じます
    ini_close(ini_file);
    
    return retStr;
}

int32_t ini_read_string2(const char *file, const char *pSection, const char *pKey, char *pStr, int datalen)
{
    //ファイルが存在するか確認します
    if (access(file, F_OK) == -1)
        return -1;
    
    //ファイルを開きます
    ini_fd_t ini_file = ini_open(file, "r", ";");
    if (ini_file == 0) {
        printf("ini_open %s error!,%s\n", file, strerror(errno));
        return -1;
    }
    
    //設定情報を読み込みます
    ini_locateHeading(ini_file, pSection);
    ini_locateKey(ini_file, pKey);

    bzero(pStr, datalen);
    int ret = ini_readString(ini_file, pStr, datalen);
    if(-1 == ret){
        printf("ini_readString error!,%s,[%s]->%s\n", strerror(errno), pSection, pKey);
    }
    
    //ファイルを閉じます
    ini_close(ini_file);
    
    return ret;
}

int32_t ini_write_int(const char *file, const char *pSection, const char *pKey, int Val)
{
    int ret = -1;
#if 0    //不要な場合があります。コメントアウトすると、ファイルが存在しない場合に強制的に作成します
    //ファイルが存在するか確認します
    if (access(file, F_OK) == -1)
        return ret;
#endif
    //ファイルを開きます
    ini_fd_t ini_file = ini_open(file, "wb", ";");
    if (ini_file == 0) {
        printf("ini_open error!,%s\n", strerror(errno));
        return ret;
    }
    
    //設定情報を書き込みます
    ini_locateHeading(ini_file, pSection);
    ini_locateKey(ini_file, pKey);
    
    int res = ini_writeInt(ini_file, Val);
    if(-1 == res){
        printf("ini_writeInt error!,%s,[%s]->%s\n", strerror(errno), pSection, pKey);
    }else{
        ret = 0;
    }
    
    //ファイルを閉じます
    ini_close(ini_file);
    
    return ret;
}


int32_t ini_write_string(const char *file, const char *pSection, const char *pKey, const char *pStr)
{
    int32_t ret = -1;
#if 0    //不要な場合があります。コメントアウトすると、ファイルが存在しない場合に強制的に作成します
    //ファイルが存在するか確認します
    if (access(file, F_OK) == -1)
        return ret;
#endif
    //ファイルを開きます
    ini_fd_t ini_file = ini_open(file, "w", ";");
    if (ini_file == 0) {
        printf("ini_open error!\n");
        return ret;
    }
    
    //設定情報を書き込みます
    ini_locateHeading(ini_file, pSection);
    ini_locateKey(ini_file, pKey);
    
    int res = ini_writeString(ini_file, pStr);
    if(-1 == res) {
        printf("ini_writeString error!,%s,[%s]->%s\n", strerror(errno), pSection, pKey);
    }else{
        ret = 0;
    }
    
    //ファイルを閉じます
    ini_close(ini_file);
    
    return ret;
}

