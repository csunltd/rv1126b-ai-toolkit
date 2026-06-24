#ifndef __RKNN_FACE_LANDMARK_H__
#define __RKNN_FACE_LANDMARK_H__

#include "opencv2/opencv.hpp"
#include "rknn_api.h"


/**< @brief face_landmarkÄ£ÐÍ½á¹¹Ìå */
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
} rknn_face_landmark_context_t;


#ifdef __cplusplus
extern "C" {
#endif

	/**
	* @brief  face_landmark98_init
	*
	* @param[i/o]		p_face_landmark			face_landmarkモデルコンテキスト
	* @param[in]		p_model_path			face_landmark rknnモデルパス
	* @return									モデル初期化結果。0 は成功、負の値は失敗を表します
	*/
	int face_landmark98_init(rknn_face_landmark_context_t *p_face_landmark, const char *p_model_path);


	/**
	* @brief  face_landmark98_run
	*
	* @param[in]		p_face_landmark			face_landmarkモデルコンテキスト
	* @param[in]		image					検出対象画像
	* @return									顔ランドマーク
	*/
	std::vector<cv::Point> face_landmark98_run(rknn_face_landmark_context_t *p_face_landmark, cv::Mat image);


	/**
	* @brief face_landmark_release
	*
	* @param[i/o]		p_face_landmark			face_landmarkモデルコンテキスト
	* @return									モデル解放結果
	*/
	int face_landmark98_release(rknn_face_landmark_context_t* p_face_landmark);


#ifdef __cplusplus
}
#endif

#endif //__RKNN_FACE_LANDMARK_H__