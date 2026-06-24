#ifndef CAR_DETECT_H
#define CAR_DETECT_H

#include "postprocess.h"
#include "rknn_api.h"
#include <opencv2/opencv.hpp>




/* 
 * 人物検出を初期化します関数
 * ctx:入力パラメータ,rknn_contextハンドル
 * path:入力パラメータ,アルゴリズムモデルパス
 */
int car_detect_init(rknn_context *ctx, const char * path);


/* 
 * 人物検出実行関数
 * ctx:入力パラメータ,rknn_contextハンドル
 * input_image:入力パラメータ,画像データ入力(cv::Matは OpenCV の型です)
 * output_dets:出力パラメータ，オブジェクト検出ボックス出力
 */
int car_detect_run(rknn_context ctx, cv::Mat input_image, detect_result_group_t *detect_result_group);


/* 
 * 人物検出を解放します関数
 * ctx:入力パラメータ,rknn_contextハンドル
 */
int car_detect_release(rknn_context ctx);




#endif
