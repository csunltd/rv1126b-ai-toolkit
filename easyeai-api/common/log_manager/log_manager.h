/**
 *
 * Copyright 2021 by Guangzhou Easy EAI Technologny Co.,Ltd.
 * website: www.easy-eai.com
 *
 * Author: Jiehao.Zhong <zhongjiehao@easy-eai.com>
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * License file for more details.
 * 
 */

#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PD_DEFAULT_LOG_PATH 	"/userdata/logs"	//ログのデフォルト保存ディレクトリ

// 出力形式を定義します（一度に 1 つのみ選択できます）
#define PRINT_TO_NONE	    0       // 何も出力しません
#define PRINT_TO_LOCAL      1       // ローカル shell に出力します
#define PRINT_TO_FILE       2       // 同名の .log ファイルに出力します

extern int32_t printLog(char const *filePath, int lineNum, char const *funcName, int logLevel, char const *logCon, va_list args);
extern int32_t preSet_defalut_log_path(const char *path);
extern void log_manager_init(const char * const strConfigPath, const char * const strModuleName);


// printfMsg はヘッダーファイル内部でのみ使用します。外部から呼び出すことは推奨しません
inline int32_t printMsg(char const *filePath, int lineNum, char const *funcName, int logLevel, char const *logCon, ...){
    va_list args;
    va_start(args, logCon); //パラメータリストのスタックポインタ位置を logCon に合わせます
    return printLog(filePath, lineNum, funcName, logLevel, logCon, args);
}
// 出力レベルを定義します
#define PRINT_LEVEL_NONE    0       // 出力情報を表示しません
#define PRINT_LEVEL_ERROR   1       // エラー情報, 否定判定時に追加します
#define PRINT_LEVEL_DEBUG   2       // デバッグ情報。デバッグ時に追加します
#define PRINT_LEVEL_TRACE   3       // スタックトレース。トレース時に追加します
#define PRINT_TRACE(str, args...) \
	printMsg(__FILE__, __LINE__, __FUNCTION__, PRINT_LEVEL_TRACE, str, ##args)
#define PRINT_DEBUG(str, args...) \
	printMsg(__FILE__, __LINE__, __FUNCTION__, PRINT_LEVEL_DEBUG, str, ##args)
#define PRINT_ERROR(str, args...) \
	printMsg(__FILE__, __LINE__, __FUNCTION__, PRINT_LEVEL_ERROR, str, ##args)

#ifdef __cplusplus
}
#endif

#endif


