#ifndef HELMET_DETECT_H
#define HELMET_DETECT_H

#include "postprocess.h"
#include "rknn_api.h"
#include <opencv2/opencv.hpp>




/* 
 * ヘルメット検出初期化関数
 * ctx:入力パラメータ,rknn_contextハンドル
 * path:入力パラメータ,アルゴリズムモデルパス
 */
int helmet_detect_init(rknn_context *ctx, const char * path);


/* 
 * ヘルメット検出実行関数
 * ctx:入力パラメータ,rknn_contextハンドル
 * input_image:入力パラメータ,画像データ入力(cv::Matは OpenCV の型です)
 * output_dets:出力パラメータ，オブジェクト検出ボックス出力
 */
int helmet_detect_run(rknn_context ctx, cv::Mat input_image, detect_result_group_t *detect_result_group);


/* 
 * ヘルメット検出解放関数
 * ctx:入力パラメータ,rknn_contextハンドル
 */
int helmet_detect_release(rknn_context ctx);




#endif
