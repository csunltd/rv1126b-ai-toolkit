#ifndef __AUDIO_UTILS_H__
#define __AUDIO_UTILS_H__

/**< @brief 音声情報 */
typedef struct
{
	float *data;
	int num_frames;
	int num_channels;
	int sample_rate;
} audio_buffer_t;


#ifdef __cplusplus
extern "C"
{
#endif

	/// 音声ファイルをバッファに読み込みます。
	int read_audio(const char *path, audio_buffer_t *audio);

	/// 音声データを WAV ファイルに保存します。
	int save_audio(const char *path, float *data, int num_frames, int sample_rate, int num_channels);

	/// 音声データを必要なサンプリングレートにリサンプリングします。
	int resample_audio(audio_buffer_t *audio, int original_sample_rate, int desired_sample_rate);

	/// 音声データをモノラルに変換します。
	int convert_channels(audio_buffer_t *audio);

#ifdef __cplusplus
} 
#endif

#endif // __AUDIO_UTILS_H__