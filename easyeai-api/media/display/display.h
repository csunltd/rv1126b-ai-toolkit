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
/** 特别注意:
 * 1. display.h与display_pro.h里面的接口大部分是互斥的，除非特别说明，否则不建议混合调用。
 */

// overlay层操作接口:
int disp_init();
void disp_release();
void window_commit(void *ptr/*default BGR888*/, int imgWidth, int imgHeight, int imgRotation);	//内部调用rga，需要加锁使用

// primary层操作接口:
int uiLayer_init();
void uiLayer_release();
void uiLayer_commit(void *ptr/*default BGR888*/, int imgWidth, int imgHeight);	//内部调用rga，需要加锁使用

//调用此接口，overlay与primary层都会释放
void disp_exit(void);

#ifdef __cplusplus
}
#endif
#endif

