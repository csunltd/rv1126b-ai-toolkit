#ifndef FACE_RECOGNITION_H
#define FACE_RECOGNITION_H

#include "rknn_api.h"


/* 
 * 顔認識を初期化します関数
 * ctx:入力パラメータ,rknn_contextハンドル
 * path:入力パラメータ,アルゴリズムモデルパス
 */
int face_recognition_init(rknn_context *ctx, const char * path);

/* 
 * 顔認識を実行します関数
 * ctx:入力パラメータ,rknn_contextハンドル
 * face_image:入力パラメータ,画像データ入力(cv::Matは OpenCV の型です)
 * feature:出力パラメータ，アルゴリズムが出力する顔特徴コード
 */
int face_recognition_run(rknn_context ctx, cv::Mat *face_image, float (*feature)[512]);

/* 
 * 顔認識特徴比較関数
 * feature_1:入力パラメータ,顔特徴コード 1
 * feature_2:入力パラメータ,顔特徴コード 2
 */
float face_recognition_comparison(float *feature_1, float *feature_2, int output_len);

/* 
 * 顔認識を解放します関数
 * ctx:入力パラメータ,rknn_contextハンドル
 */
int face_recognition_release(rknn_context ctx);

#endif
