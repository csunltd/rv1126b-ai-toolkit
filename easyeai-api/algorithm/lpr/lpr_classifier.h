#ifndef __LPR_CLASSIFIER_H__
#define __LPR_CLASSIFIER_H__

#include "rknn_api.h"
#include "opencv2/opencv.hpp"

/**< @brief ナンバープレート分類器 */
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
}rknn_lpr_classifer_t;

 
#ifdef __cplusplus
extern "C" {
#endif

	/**
	* @brief  rknn ナンバープレート分類を初期化します関数
	*
	* @param[in]		p_model_path			ナンバープレート分類器モデルパス
	* @param[i/o]		p_cls					ナンバープレート分類器モデルコンテキスト
	* @return									モデル初期化結果。0 は成功、負の値は失敗を表します
	*/
	int rknn_lpr_classifer_init(const char *p_model_path, rknn_lpr_classifer_t *p_cls);


	/**
	* @brief  rknn ナンバープレート分類認識関数
	*
	* @param[in]		image					分類対象画像（BGR 形式）
	* @param[i/o]		p_cls					ナンバープレート分類器モデルコンテキスト
	* @param[i/o]		label					クラベル：クラス数は 3、0 青／1 緑／2 黄
	* @param[i/o]		score					信頼度スコア
	* @return									モデル初期化結果。0 は成功、負の値は失敗を表します
	*/
	int rknn_lpr_classifer_calc(cv::Mat image, rknn_lpr_classifer_t *p_cls, int &label, float &score);


	/**
	* @brief  rknn  ナンバープレート分類を解放します関数
	*
	* @param[i/o]		p_cls					ナンバープレート分類器モデルコンテキスト
	* @return									モデル解放結果
	*/
	int rknn_lpr_classifer_deinit(rknn_lpr_classifer_t *p_cls);


#ifdef __cplusplus
}
#endif

#endif //__LPR_CLASSIFIER_H__