/**
 *
 * Copyright 2025 by Guangzhou Easy EAI Technologny Co.,Ltd.
 * website: www.easy-eai.com
 *
 * Author: ZJH <zhongjiehao@easy-eai.com>
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * License file for more details.
 * 
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "display_pro.h"
/** 特に注意:
 * 1. display.hと display_pro.h 内のインターフェースの大部分は排他的です。特に明記されていない限り、混在して呼び出すことは推奨しません。
 */

// overlay層操作インターフェース:
int disp_init();
void disp_release();
void window_commit(void *ptr/*default BGR888*/, int imgWidth, int imgHeight, int imgRotation);	//内部で RGA を呼び出すため、ロックして使用する必要があります

// primary層操作インターフェース:
int uiLayer_init();
void uiLayer_release();
void uiLayer_commit(void *ptr/*default BGR888*/, int imgWidth, int imgHeight);	//内部で RGA を呼び出すため、ロックして使用する必要があります

//このインターフェースを呼び出すと、overlay 層と primary 層の両方が解放されます
void disp_exit(void);

#ifdef __cplusplus
}
#endif
#endif

