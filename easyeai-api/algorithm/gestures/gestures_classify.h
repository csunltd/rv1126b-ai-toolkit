#ifndef __RKNN_MOBILENET_V4_H__
#define __RKNN_MOBILENET_V4_H__

#include "rknn_api.h"
#include "opencv2/opencv.hpp"

/**< @brief mobilenet v4画像分類 */
typedef struct {
    rknn_context rknn_ctx;
    rknn_input_output_num io_num;
    rknn_tensor_attr* input_attrs;
    rknn_tensor_attr* output_attrs;
    int model_channel;
    int model_width;
    int model_height;
} rknn_gestures_classify_context_t;


#ifdef __cplusplus
extern "C" {
#endif

   /**
	* @brief  rknn gestures_classifyモデル初期化
	*
	* @param[in]		p_model_path			gestures_classify rknnモデルパス
	* @param[i/o]		p_mv4			        gestures_classify モデルコンテキスト
	* @return									モデル初期化結果。0 は成功、負の値は失敗を表します
	*/
    int rknn_gestures_classify_init(const char *p_model_path, rknn_gestures_classify_context_t *p_mv4);


   /**
	* @brief  rknn gestures_classify画像分類
	*
    * @param[in]		p_mv4			        gestures_classifyモデルコンテキスト
	* @param[in]		image					対象分類画像
	* @param[i/o]		label			        分類クラス
	* @param[i/o]		score			        分類信頼度
	* @return									計算結果ステータス。0 は成功、負の値は失敗を表します
	*/
    int rknn_gestures_classify_calc(rknn_gestures_classify_context_t *p_mv4, cv::Mat image, int &label, float &score);


   /**
	* @brief  rknn gestures_classifyモデル解放
	*
	* @param[i/o]		p_mv4			        gestures_classifyモデルコンテキスト
	* @return									モデル解放結果。0 は成功、負の値は失敗を表します
	*/
    int rknn_gestures_classify_deinit(rknn_gestures_classify_context_t *p_mv4);


#ifdef __cplusplus
}
#endif

#endif //__RKNN_MOBILENET_V4_H__