#ifndef __GESTURES_H__
#define __GESTURES_H__

#include "gestures_classify.h"
#include "gestures_pose.h"

/**< @brief ジェスチャー認識構造体 */
typedef struct {
    rknn_gestures_pose_context_t  *p_gestures;
    rknn_gestures_classify_context_t  *p_mv4;
}rknn_gestures_context_t;


/**< @brief ジェスチャー認識結果 */
typedef struct {
	int		gesture;            // ジェスチャークラス
	int		left;               // 手部検出框
	int		top;
	int		right;
	int		bottom;			
	float	score;			    // 信頼度
	float	keypoints[21][2];	// keypoints x,y
} rknn_gestures_result_t;


#ifdef __cplusplus
extern "C" {
#endif

   /**
	* @brief  ジェスチャー認識を初期化します
	*
    * @param[in]		p_gesture_pose_path			ジェスチャーキーポイントモデルパス
	* @param[in]		p_gesture_classify_path		ジェスチャー姿勢モデルパス
	* @param[i/o]		gestures			    ジェスチャー認識コンテキスト
	* @param[in]		cls_num			クラス数
	* @return									モデル初期化結果。0 は成功、負の値は失敗を表します
	*/
    int gestures_init(const char *p_gestures_path, const char *p_mobilenet_path, rknn_gestures_context_t &gestures, int cls_num);


   /**
	* @brief  ジェスチャー認識関数
	*
    * @param[in]		gestures			    ジェスチャー認識コンテキスト
    * @param[in]		image			        対象認識画像
	* @param[in]		conf_threshold		    gesturesオブジェクト検出信頼度
	* @param[in]		nms_threshold		    gestures非最大抑制しきい値
	* @return										 gestures検出結果
	*/
    std::vector<rknn_gestures_result_t> gestures_run(rknn_gestures_context_t &gestures, cv::Mat image, float conf_threshold, float nms_threshold);


    /**
	* @brief  ジェスチャー認識解放
	*
	* @param[i/o]		p_gestures			    ジェスチャー認識コンテキスト
	* @return									モデル解放結果。0 は成功、負の値は失敗を表します
	*/
    int gestures_release(rknn_gestures_context_t &gestures);


#ifdef __cplusplus
}
#endif

#endif // __RKNN_GESTURES_H__