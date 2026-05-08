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

// 屏幕操作接口:
// 返回值: 0-成功，非0-失败
int  screen_init(void);
// 返回值: 0-成功，非0-失败
int  screen_info(int *width, int *height, int *refresh);
void screen_exit(void);

// 显示区域操作接口:
typedef struct {
	int x_off;
	int y_off;
    int width;
	int height;
} display_t;
// 返回值: 0-成功，非0-失败
int disp_init_pro(display_t *pDisplay);
void disp_release_pro();
// 返回值: 0-成功，非0-失败
int uiLayer_init_pro(display_t *pDisplay);
void uiLayer_release_pro();

// 窗口操作接口:
#define DISPLAY_WIN_REG_MAX 16
typedef struct {
	int zpos;	//窗口层级，用于确定窗口的显示顺序，zpos越大，窗口越靠前，不能超过DISPLAY_WIN_REG_MAX
	int win_x;	//窗口相对于display区域的起始X坐标
	int win_y;	//窗口相对于display区域的起始Y坐标
	int win_w;	//窗口区域宽度
	int win_h;	//窗口区域高度
} window_t;
#define DISPLAY 0
#define UILAYER 1
// 返回值: 成功返回窗口索引，失败返回[负值]
int add_window_to(int layer, window_t *window);
// 返回值: 成功返回窗口索引，失败返回[负值]
int remove_window_from(int layer, int win_chn);
bool window_is_used(int layer, int win_chn);
/* DISPLAY 和 UILAYER 的 win_chn 各自独立，可以相同，范围是0..DISPLAY_WIN_REG_MAX-1*/

/**
 * Zero-copy 显示帧描述（DMA-BUF + RGA 像素格式）。
 * - dmabuf_fd: 已由解码器/相机/allocator 导出的 fd（调用方在 commit 返回前须保持有效，勿 close）。
 * - pitch_bytes: 首平面一行字节跨度（与 DRM 的 pitch 一致）。
 * - rga_format: 与 Rockchip RgaApi.h 中 RK_FORMAT_* 取值一致（如 RK_FORMAT_BGR_888、RK_FORMAT_YCbCr_420_SP）。
 */
typedef struct display_dmabuf_frame {
    int dmabuf_fd;
    int width;
    int height;
    int pitch_bytes;
	//int HorStride;	//输入图像水平步长
	//int VirStride;	//输入图像垂直步长
	int rotation;	    //顺时针旋转输入图像角度，支持90、180、270、0度(若旋转和裁切搭配使用，处理顺序为先裁切后旋转)
    uint32_t rga_format;
#if 0
	/* 如果不设置crop系列参数，则默认拉伸原图铺满整个win ;
	 *设置crop则先裁切再把裁切后的图像铺满整个win，以便保持图像宽高比例
	 */
	int crop_x;		//裁切起始X坐标(基于旋转前图像的坐标系)
	int crop_y;		//裁切起始Y坐标(基于旋转前图像的坐标系)
	int crop_w;		//裁切后的图像宽度(以旋转前的图像为参考)
	int crop_h;		//裁切后的图像高度(以旋转前的图像为参考)
#endif
} display_dmabuf_frame_t;
/**
 * 单 plane 多窗口：
 * window_commit_pro / uiLayer_commit_pro 仅把帧写入内部槽并标脏，不做 RGA。
 * window_refresh_pro / uiLayer_refresh_pro 内按 zpos RGA 合成再 drm_commit。
 * BUF_COUNT==1 时仅用 drmBuff[0]；BUF_COUNT>1 时在 drmBuff 间轮换，合成前整屏拷贝上一帧 fb，避免增量脏区与换缓冲不一致。
 */
int window_commit_pro(int win_chn, const display_dmabuf_frame_t *frame);
int window_refresh_pro(void);	//内部调用rga，需要加锁使用
int uiLayer_commit_pro(int win_chn, const display_dmabuf_frame_t *frame);
int uiLayer_refresh_pro(void);	//内部调用rga，需要加锁使用
//------------------------------------------------------------------------------------------------------------------------------


// 以下是公共接口，允许在调用display.h接口时被混合调用：
void set_uiLayer_on_top(bool onTop);
void set_alpha_blend_mode(int mode); // [0:premultiplied, 1:Non-premultiplied]

#ifdef __cplusplus
}
#endif
#endif

