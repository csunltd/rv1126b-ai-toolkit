#ifndef __LPR_H__
#define __LPR_H__

#include "lpr_detector.h"
#include "lpr_classifier.h"
#include "lpr_recognizer.h"

/**< @brief lpr ナンバープレート認識 */
typedef struct {
	rknn_lpr_detector_t		det;	// ナンバープレート検出モデル
	rknn_lpr_classifer_t	cls;	// ナンバープレート分類モデル
	rknn_lpr_recognizer_t   rec;	// ナンバープレート認識モデル
}rknn_lpr_t;


/**< @brief lpr ナンバープレート認識結果 */
typedef struct {
	cv::Rect					box;				// 検出ボックス位置
	int                         color;				// ナンバープレート色：0 青色、1 緑色、2 黄色
	int							layer_num;			// ナンバープレート行数。0 は 1 行、1 は 2 行を表します
	float                       det_score;			// ナンバープレート検出信頼度
	float                       cls_score;			// ナンバープレート分類信頼度
	float                       reg_score;			// 文字認識信頼度
	std::vector<std::string>    char_list;			// ナンバープレート文字
	cv::Point					key_pts[4];			// ナンバープレートの 4 つの角点
}rknn_lpr_result_t;



#ifdef __cplusplus
extern "C" {
#endif

	/**
	* @brief  rknn lpr ナンバープレート認識を初期化します関数
	*
	* @param[in]		p_det_model				ナンバープレート検出モデルパス
	* @param[in]		p_cls_model				ナンバープレート分類モデルパス
	* @param[in]		p_rec_model				ナンバープレート認識モデルパス
	* @param[i/o]		p_lpr					lprナンバープレート認識
	* @return									モデル初期化結果。0 は成功、負の値は失敗を表します
	*/
	int lpr_init(const char *p_det_model, const char *p_cls_model, const char *p_rec_model, rknn_lpr_t *p_lpr);


	/**
	* @brief  LPR ナンバープレート認識関数
	*
	* @param[in]		image					分類対象画像（BGR 形式）
	* @param[i/o]		p_lpr					lprナンバープレート認識
	* @param[in]		conf_thresh				信頼度しきい値
	* @param[in]		nms_thresh				NMS しきい値
	* @return									ナンバープレート認識結果を返します
	*/
	std::vector<rknn_lpr_result_t> lpr_run(cv::Mat image, rknn_lpr_t *p_lpr, float conf_thresh, float nms_thresh);


	/**
	* @brief  LPR ナンバープレート認識を解放します関数
	*
	* @param[i/o]		p_lpr					ナンバープレート分類器モデルコンテキスト
	* @return									モデル解放結果
	*/
	int lpr_release(rknn_lpr_t *p_lpr);

#ifdef __cplusplus
}
#endif


#endif