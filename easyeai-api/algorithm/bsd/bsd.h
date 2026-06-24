#ifndef BSD_H
#define BSD_H

#include "bsd_postprocess.h"
#include "rknn_api.h"
#include <opencv2/opencv.hpp>




/* 
 * bsd初期化関数
 * ctx:入力パラメータ,rknn_contextハンドル
 * path:入力パラメータ,アルゴリズムモデルパス
 */
int bsd_init(rknn_context *ctx, const char * path);


/* 
 * bsd実行関数
 * ctx:入力パラメータ,rknn_contextハンドル
 * input_image:入力パラメータ,画像データ入力(cv::Matは OpenCV の型です)
 * output_dets:出力パラメータ，オブジェクト検出ボックス出力
 */
int bsd_run(rknn_context ctx, cv::Mat input_image, detect_result_group_t *detect_result_group);


/* 
 * bsd解放関数
 * ctx:入力パラメータ,rknn_contextハンドル
 */
int bsd_release(rknn_context ctx);




#endif
