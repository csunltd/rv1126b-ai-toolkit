#ifndef __PERSON_POSE_H__
#define __PERSON_POSE_H__

#include "opencv2/opencv.hpp"
#include "rknn_api.h"



/**< @brief person poseモデル構造体 */
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
} person_pose_context_t;


/**< @brief person pose検出およびセグメンテーション結果 */
typedef struct {
	int		cls_id;
	int		left;
	int		top;
	int		right;
	int		bottom;			
	float	prop;			
	float	keypoints[17][3];	//keypoints x,y,conf
} person_pose_result_t;


#ifdef __cplusplus
extern "C" {
#endif

	/**
	* @brief  person pose 初期化関数
	*
	* @param[in]		p_model_path			pose init rknnモデルパス
	* @param[i/o]		p_person_pose			person poseモデルコンテキスト
	* @param[in]		cls_num					クラス数
	* @return									モデル初期化結果。0 は成功、負の値は失敗を表します
	*/
	int person_pose_init(const char *p_model_path, person_pose_context_t *p_person_pose, int cls_num);


	/**
	* @brief  person pose 実行関数
	*
	* @param[in]		image					検出対象画像
	* @param[in]		p_person_pose			person poseモデルコンテキスト
	* @param[in]		nms_threshold			NMS しきい値
	* @param[in]		conf_threshold			信頼度しきい値
	* @return									検出および姿勢推定結果
	*/
	std::vector<person_pose_result_t> person_pose_run(cv::Mat image, person_pose_context_t *p_person_pose, float nms_threshold, float conf_threshold);


	/**
	* @brief  rknn person pose解放関数
	*
	* @param[i/o]		p_person_pose			person poseモデルコンテキスト
	* @return									モデル解放結果
	*/
	int person_pose_release(person_pose_context_t* p_person_pose);


#ifdef __cplusplus
}
#endif

#endif //__PERSON_POSE_H__