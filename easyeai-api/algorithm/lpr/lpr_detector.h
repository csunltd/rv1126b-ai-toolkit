#ifndef __LPR_DETECTOR_H__
#define __LPR_DETECTOR_H__

#include "rknn_api.h"
#include "opencv2/opencv.hpp"

/**< @brief ナンバープレート検出器 */
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
	int output_width;
	int output_height;
	int32_t zp;
	float scale;
	bool is_quant;
}rknn_lpr_detector_t;


/**< @brief ナンバープレート検出結果 */
typedef struct {
	cv::Rect	box;			// 検出ボックス位置
	int         layer_num;      // ナンバープレート行数。0 は 1 行、1 は 2 行を表します
	float       score;          // 信頼度
	cv::Point   key_pts[4];     // ナンバープレートの 4 つの角点
}rknn_lpr_det_result_t;


#ifdef __cplusplus
extern "C" {
#endif

	/**
	* @brief  rknn ナンバープレート検出器初期化関数
	*
	* @param[in]		p_model_path			ナンバープレート検出器モデルパス
	* @param[i/o]		p_det					ナンバープレート検出器モデルコンテキスト
	* @return									モデル初期化結果。0 は成功、負の値は失敗を表します
	*/
	int rknn_lpr_detector_init(const char *p_model_path, rknn_lpr_detector_t *p_det);


	/**
	* @brief  rknn ナンバープレート検出関数
	*
	* @param[in]		image					検出対象ナンバープレート画像（BGR 形式）
	* @param[i/o]		p_det					ナンバープレート検出器モデルコンテキスト
	* @param[in]		conf_thresh				信頼度しきい値
	* @param[in]		nms_thresh				NMS しきい値
	* @return									検出結果と座標を返します
	*/
	std::vector<rknn_lpr_det_result_t> rknn_lpr_detector_calc(cv::Mat image, rknn_lpr_detector_t *p_det, float conf_thresh, float nms_thresh);


	/**
	* @brief  rknn  ナンバープレート検出器解放関数
	*
	* @param[i/o]		p_det					ナンバープレート検出器モデルコンテキスト
	* @return									モデル解放結果
	*/
	int rknn_lpr_detector_deinit(rknn_lpr_detector_t *p_det);


#ifdef __cplusplus
}
#endif

#endif //__LPR_DETECTOR_H__