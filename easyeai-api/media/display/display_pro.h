/**
 *
 * Copyright 2025 by Guangzhou Easy EAI Technologny Co.,Ltd.
 * website: www.easy-eai.com
 *
 * Author: ZJH <zhongjiehao@easy-eai.com>
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * License file for more details.
 * 
 */

#ifndef DISPLAY_PRO_H
#define DISPLAY_PRO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// 画面操作インターフェース:
// 戻り値: 0 は成功，非 0 は失敗
int  screen_init(void);
// 戻り値: 0 は成功，非 0 は失敗
int  screen_info(int *width, int *height, int *refresh);
void screen_exit(void);

// 表示領域操作インターフェース:
typedef struct {
	int x_off;
	int y_off;
    int width;
	int height;
} display_t;
// 戻り値: 0 は成功，非 0 は失敗
int disp_init_pro(display_t *pDisplay);
void disp_release_pro();
// 戻り値: 0 は成功，非 0 は失敗
int uiLayer_init_pro(display_t *pDisplay);
void uiLayer_release_pro();

// ウィンドウ操作インターフェース:
#define DISPLAY_WIN_REG_MAX 16
typedef struct {
	int zpos;	//ウィンドウ階層。ウィンドウの表示順序を決定します。zpos が大きいほど前面に表示され、DISPLAY_WIN_REG_MAX を超えてはいけません
	int win_x;	//display 領域に対するウィンドウ開始 X 座標
	int win_y;	//display 領域に対するウィンドウ開始 Y 座標
	int win_w;	//ウィンドウ領域の幅
	int win_h;	//ウィンドウ領域の高さ
} window_t;
#define DISPLAY 0
#define UILAYER 1
// 戻り値: 成功時はウィンドウインデックスを返し、失敗時は［負の値］を返します
int add_window_to(int layer, window_t *window);
// 戻り値: 成功時はウィンドウインデックスを返し、失敗時は［負の値］を返します
int remove_window_from(int layer, int win_chn);
bool window_is_used(int layer, int win_chn);
/* DISPLAY と UILAYER の win_chn はそれぞれ独立しており、同じ値でも構いません。範囲は 0..DISPLAY_WIN_REG_MAX-1 です*/

/**
 * Zero-copy 表示フレーム説明（DMA-BUF + RGA ピクセル形式）。
 * - dmabuf_fd: デコーダ／カメラ／allocator からエクスポート済みの fd です（呼び出し側は commit が戻るまで有効に保持し、close しないでください）。
 * - pitch_bytes: 先頭 plane の 1 行あたりのバイトストライドです（DRM の pitch と一致します）。
 * - rga_format: Rockchip RgaApi.h 内の RK_FORMAT_* の値と一致します（例：RK_FORMAT_BGR_888、RK_FORMAT_YCbCr_420_SP）。
 */
typedef struct display_dmabuf_frame {
    int dmabuf_fd;
    int width;
    int height;
    int pitch_bytes;
	//int HorStride;	//入力画像の水平ストライド
	//int VirStride;	//入力画像の垂直ストライド
	int rotation;	    //入力画像を時計回りに回転する角度です。90、180、270、0 度に対応します（回転とクロップを併用する場合、処理順序はクロップ後に回転です）
    uint32_t rga_format;
#if 0
	/* crop 系パラメータを設定しない場合、デフォルトでは元画像を引き伸ばして win 全体に表示します ;
	 *crop を設定した場合は、先にクロップしてからクロップ後の画像を win 全体に表示し、画像のアスペクト比を維持します
	 */
	int crop_x;		//クロップ開始 X 座標（回転前画像の座標系基準）
	int crop_y;		//クロップ開始 Y 座標（回転前画像の座標系基準）
	int crop_w;		//クロップ後の画像幅（回転前画像を基準）
	int crop_h;		//クロップ後の画像高さ（回転前画像を基準）
#endif
} display_dmabuf_frame_t;
/**
 * 単一 plane のマルチウィンドウ：
 * window_commit_pro / uiLayer_commit_pro フレームを内部スロットへ書き込み、dirty としてマークするだけで、RGA は実行しません。
 * window_refresh_pro / uiLayer_refresh_pro 内部で zpos に従って RGA 合成を行い、その後 drm_commit します。
 * BUF_COUNT==1 の場合は drmBuff[0]；BUF_COUNT>1 の場合は drmBuff 間でローテーションし、合成前に前フレームの fb を全画面コピーして、差分 dirty 領域とバッファ切り替えの不一致を避けます。
 */
int window_commit_pro(int win_chn, const display_dmabuf_frame_t *frame);
int window_refresh_pro(void);	//内部で RGA を呼び出すため、ロックして使用する必要があります
int uiLayer_commit_pro(int win_chn, const display_dmabuf_frame_t *frame);
int uiLayer_refresh_pro(void);	//内部で RGA を呼び出すため、ロックして使用する必要があります
//------------------------------------------------------------------------------------------------------------------------------


// 以下は共通インターフェースであり、display.h のインターフェース呼び出し時に混在して呼び出すことができます：
void set_uiLayer_on_top(bool onTop);
void set_alpha_blend_mode(int mode); // [0:premultiplied, 1:Non-premultiplied]

#ifdef __cplusplus
}
#endif
#endif

