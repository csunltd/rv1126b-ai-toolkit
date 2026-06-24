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

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>


#if defined(__cplusplus)
extern "C" {
#endif

typedef struct{
    float x;
    float y;
}fPoint_t;
typedef struct{
    float left;   //x1
    float top;    //y1
    float right;  //x2
    float bottom; //y2
}fRect_t;

typedef struct{
    int32_t x;
    int32_t y;
}s32Point_t;
typedef struct{
    int32_t left;   //x1
    int32_t top;    //y1
    int32_t right;  //x2
    int32_t bottom; //y2
}s32Rect_t;

/* 機能：点が矩形内にあるか判定します
 * 
 */
extern bool point_in_rect(s32Point_t point, s32Rect_t rect);

/* 機能：矩形の面積を計算します
 * 
 */
extern int32_t calc_rect_square(s32Rect_t rect);

/* 機能：面積が小さい矩形を取得します
 * 
 */
extern s32Rect_t min_rect(s32Rect_t rect1, s32Rect_t rect2);

/* 機能：面積が大きい矩形を取得します
 * 
 */
extern s32Rect_t max_rect(s32Rect_t rect1, s32Rect_t rect2);

/* 機能：矩形同士が交差または接しているか判定します
 * 
 */
extern bool rect_is_intersect(s32Rect_t rect1, s32Rect_t rect2);

/* 機能：2 つの矩形の交差部分の面積を計算します（接しているだけの場合、面積も 0 です）
 * 
 */
extern int32_t calc_rect_intersect_square(s32Rect_t rect1, s32Rect_t rect2);

/* 機能：［2 つの矩形の交差部分の面積］と［小さい矩形の面積］の比率を計算します
 * 
 */
extern double calc_intersect_of_min_rect(s32Rect_t rect1, s32Rect_t rect2);

/* 機能：2 つの矩形の IoU を計算します
 * 
 */
extern double calc_intersect_of_union(s32Rect_t rect1, s32Rect_t rect2);

#if defined(__cplusplus)
}
#endif
#endif // GEOMETRY_H