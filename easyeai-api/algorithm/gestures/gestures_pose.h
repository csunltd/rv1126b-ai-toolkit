#ifndef __GESTURES_POSE_H__
#define __GESTURES_POSE_H__

#include "opencv2/opencv.hpp"
#include "rknn_api.h"



/**< @brief gestures poseモデル構造体*/
typedef struct {
    rknn_context rknn_ctx;
    rknn_input_output_num io_num;
    rknn_tensor_attr* input_attrs;
    rknn_tensor_attr* output_attrs;
    int model_channel;
    int model_width;
    int model_height;
    int input_image_width;
    int input_image_height;
    bool is_quant;
	int  cls_num;
} rknn_gestures_pose_context_t;


/**< @brief gestures pose検出およびセグメンテーション結果 */
typedef struct {
	int		cls_id;
	int		left;
	int		top;
	int		right;
	int		bottom;			
	float	prop;			
	float	keypoints[21][3];	//keypoints x,y,conf
} rknn_gestures_pose_result_t;


#ifdef __cplusplus
extern "C" {
#endif

	/**
	* @brief  rknn gestures pose 初期化関数
	*
	* @param[in]		p_model_path			gestures pose rknnモデルパス
	* @param[i/o]		p_gestures_pose			gestures poseモデルコンテキスト
	* @param[in]		cls_num					クラス数
	* @return									モデル初期化結果。0 は成功、負の値は失敗を表します
	*/
	int rknn_gestures_pose_init(const char *p_model_path, rknn_gestures_pose_context_t *p_gestures_pose, int cls_num);


	/**
	* @brief  rknn gestures pose計算関数
	*
	* @param[in]		image					検出対象画像
	* @param[in]		p_gestures_pose			gestures poseモデルコンテキスト
	* @param[in]		nms_threshold			NMS しきい値
	* @param[in]		conf_threshold			信頼度しきい値
	* @return									検出および姿勢推定結果
	*/
	std::vector<rknn_gestures_pose_result_t> rknn_gestures_pose_calc(cv::Mat image, rknn_gestures_pose_context_t *p_gestures_pose, float nms_threshold, float conf_threshold);


	/**
	* @brief  rknn gestures pose解放関数
	*
	* @param[i/o]		p_gestures_pose			gestures poseモデルコンテキスト
	* @return									モデル解放結果
	*/
	int rknn_gestures_pose_deinit(rknn_gestures_pose_context_t* p_gestures_pose);


#ifdef __cplusplus
}
#endif

#endif //__RKNN_gestures_POSE_H__