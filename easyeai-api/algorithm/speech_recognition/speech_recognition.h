#ifndef __WHISPER_H__
#define __WHISPER_H__

#include <vector>
#include <string>
#include "rknn_api.h"
#include "audio_utils.h"

#define ENABLE_NEON			1							// NEON アクセラレーションを使用します
#define M_PI				3.14159265358979323846		// pi

#define VOCAB_NUM			51865						// 語彙数
#define MAX_TOKENS			12
#define N_MELS				80							// メルスペクトルの帯域数
#define MELS_FILTERS_SIZE	201							// (N_FFT / 2 + 1)
#define SAMPLE_RATE			16000						// 音声サンプリングレート
#define CHUNK_LENGTH		30 							// 1 回で処理する音声チャンク長（単位：秒）
#define HOP_LENGTH			160							// メルスペクトルのフレームシフト
#define MAX_AUDIO_LENGTH	CHUNK_LENGTH * SAMPLE_RATE
#define N_FFT				400
#define ENCODER_INPUT_SIZE CHUNK_LENGTH * 100
#define ENCODER_OUTPUT_SIZE CHUNK_LENGTH * 50 * 768 //768		// 384/512/1024 for tiny/base/medium models respectively
#define DECODER_INPUT_SIZE ENCODER_OUTPUT_SIZE


/**< @brief 語彙情報 */
typedef struct{
	int index;
	char *token;
}vocab_entry_t;

/**< @brief Whisper モデルコンテキスト */
typedef struct{
	rknn_context			rknn_ctx;
	rknn_input_output_num	io_num;
	rknn_tensor_attr		*input_attrs;
	rknn_tensor_attr		*output_attrs;
} rknn_whisper_context_t;


/**< @brief Whisper モデル構造体 */
typedef struct {
	rknn_whisper_context_t	encoder_context;		// 音声エンコーダ（音声を言語生成に適した中間表現へ変換）
	rknn_whisper_context_t	decoder_context;		// 音声デコーダ（音声認識）
	vocab_entry_t			vocabs[VOCAB_NUM];		// 語彙表
	float					*p_mel_filters;			// フィルタ
	float					*p_encoder_output;		// エンコード出力
} rknn_whisper_t;


#ifdef __cplusplus
extern "C" {
#endif
	/**
	* @brief  speech_recognition_init 初期化関数
	*
	* @param[in]		p_encoder_path			エンコーダモデル
	* @param[in]		p_decoder_path			デコーダモデル
	* @param[in]		p_filter_path			フィルタスペクトル
	* @param[in]		p_vocab_path			語彙ファイル
	* @param[i/o]		p_whisper				音声認識オブジェクト
	* @return									ステータスコード。0 は成功を表します
	*/
	int speech_recognition_init(const char *p_encoder_path, const char *p_decoder_path, const char *p_filter_path, 
		const char *p_vocab_path, rknn_whisper_t *p_whisper);


	/**
	* @brief  speech_recognition_run 関数
	*
	* @param[in]								音声認識オブジェクト
	* @param[in]		audio					認識対象音声情報
	* @param[in]		task_code				音声認識タスク
	* @param[i/o]		recognized_text			音声認識結果
	* @return									ステータスコード。0 は成功を表します
	*/
	int speech_recognition_run(rknn_whisper_t *p_whisper, audio_buffer_t audio, int task_code, std::vector<std::string> &recognized_text);


	/**
	* @brief  speech_recognition_release 解放関数
	*
	* @param[i/o]							    音声認識オブジェクト
	* @return									モデル解放結果
	*/
	int speech_recognition_release(rknn_whisper_t *p_whisper);


#ifdef __cplusplus
}
#endif


#endif // !__WHISPER_H__
