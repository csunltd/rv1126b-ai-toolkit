#ifndef __LPR_RECOGNIZER_H__
#define __LPR_RECOGNIZER_H__

#include "rknn_api.h"
#include "opencv2/opencv.hpp"

/**< @brief ナンバープレート認識器 */
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
	bool is_quant;
}rknn_lpr_recognizer_t;


#ifdef __cplusplus
extern "C" {
#endif

	/**
	* @brief  rknn ナンバープレート文字認識を初期化します関数
	*
	* @param[in]		p_model_path			ナンバープレート文字認識モデルパス
	* @param[i/o]		p_rec					ナンバープレート文字認識モデルコンテキスト
	* @return									モデル初期化結果。0 は成功、負の値は失敗を表します
	*/
	int rknn_lpr_recognizer_init(const char *p_model_path, rknn_lpr_recognizer_t *p_rec);


	/**
	* @brief  rknn ナンバープレート文字認識関数
	*
	* @param[in]		image					認識対象ナンバープレート画像（BGR 形式）
	* @param[i/o]		p_rec					ナンバープレート文字認識モデルコンテキスト
	* @param[i/o]		char_list				ナンバープレート番号
	* @param[i/o]		score					信頼度スコア
	* @return									モデル初期化結果。0 は成功、負の値は失敗を表します
	*/
	int rknn_lpr_recognizer_calc(cv::Mat image, rknn_lpr_recognizer_t *p_rec, std::vector<std::string> &char_list, float &score);


	/**
	* @brief  rknn  ナンバープレート文字認識を解放します関数
	*
	* @param[i/o]		p_rec					ナンバープレート文字認識モデルコンテキスト
	* @return									モデル解放結果
	*/
	int rknn_lpr_recognizer_deinit(rknn_lpr_recognizer_t *p_rec);



#ifdef __cplusplus
}
#endif

#endif //__LPR_RECOGNIZER_H__