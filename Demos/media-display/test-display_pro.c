#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <rga/RgaApi.h>
#include "rga_wrapper.h" //表示を高速化するには、DMA メモリおよび RGA のゼロコピー機能を使用する必要があります

#include "display_pro.h"

#define IMAGE_PATH "720X1280.bgr"
#define IMG_WIDTH  720
#define IMG_HEIGHT 1280
#define IMG_BPP    3
#define IMAGE_SIZE ((size_t)IMG_WIDTH * IMG_HEIGHT * IMG_BPP)

#define NUM_WINDOWS 3

/**
 * DMA-BUF を割り当て、720x1280 BGR888 テスト画像を読み込み（mmap アドレス）、window_commit_pro のゼロコピー処理に使用します。
 * 成功時は 0 を返し、*dmabuf_fd、*pBuf_Map、*out_size を設定します。失敗時は -1 を返します。
 */
static int load_test_image(int *dmabuf_fd, void **pBuf_Map, size_t *out_size)
{
	int fd = -1;
	void *pBuffer = NULL;
	if (alloc_dmabuf(IMAGE_SIZE, &fd, &pBuffer) != 0) {
		fprintf(stderr, "alloc_dmabuf failed (%zu bytes)\n", IMAGE_SIZE);
		return -1;
	}

	FILE *fp = fopen(IMAGE_PATH, "rb");
	if (!fp) {
		perror(IMAGE_PATH);
		munmap(pBuffer, IMAGE_SIZE);
		close(fd);
		return -1;
	}

	if (fread(pBuffer, 1, IMAGE_SIZE, fp) != IMAGE_SIZE) {
		fprintf(stderr, "fread %s: expected %zu bytes\n", IMAGE_PATH, IMAGE_SIZE);
		fclose(fp);
		munmap(pBuffer, IMAGE_SIZE);
		close(fd);
		return -1;
	}
	fclose(fp);

	if (dma_sync_cpu_to_device(fd) != 0)
		perror("dma_sync_cpu_to_device");

	*dmabuf_fd = fd;
	*pBuf_Map = pBuffer;
	if (out_size)
		*out_size = IMAGE_SIZE;
	return 0;
}

int main(int argc, char *argv[])
{
	int sw = 0, sh = 0, refresh = 0;
	display_dmabuf_frame_t frame;

	(void)argc;
	(void)argv;
	// 1.後続の表示でゼロコピーを使用できるように DMA メモリを割り当て、テスト画像を事前に読み込みます
	int dmabuf_fd = -1;
	void *img_va = NULL;
	size_t img_size = 0;
	if (load_test_image(&dmabuf_fd, &img_va, &img_size)) {
		return -1;
	}
	
	// 2.画面ハードウェアを初期化し、display 領域を作成します
	if (0 != screen_init()){
		goto exit;
	}
	if (0 == screen_info(&sw, &sh, &refresh)){
		display_t display_dev = {0, 0, sw, sh};
		if (0 != disp_init_pro(&display_dev)) {
			goto exit2;
		}
	}

	for (int i = 0; i < NUM_WINDOWS; i++) {
		window_t w;
		memset(&w, 0, sizeof(w));
		w.zpos = i;
		if(i == 0){
			w.win_x = 300;
			w.win_y = 400;
		}else if(i == 1){
			w.win_x = 200;
			w.win_y = 150;
		}else if(i == 2){
			w.win_x = 50;
			w.win_y = 300;
		}
		w.win_w = 360;
		w.win_h = 640;

		if (add_window_to(DISPLAY, &w) < 0) {
			fprintf(stderr, "add_window_to DISPLAY chn %d failed\n", i);
		}
	}

	memset(&frame, 0, sizeof(frame));
	frame.dmabuf_fd = dmabuf_fd;
	frame.width = IMG_WIDTH;
	frame.height = IMG_HEIGHT;
	frame.pitch_bytes = IMG_WIDTH * IMG_BPP;
	frame.rotation = 0;//HAL_TRANSFORM_ROT_270;
	frame.rga_format = RK_FORMAT_BGR_888;
	for (int i = 0; i < NUM_WINDOWS; i++) {
		if (window_commit_pro(i, &frame) != 0)
			fprintf(stderr, "window_commit_pro(%d) failed\n", i);
	}
    window_commit_pro(0, &frame); //--表示順序が想定どおりか確認するために使用します

	if (window_refresh_pro() != 0)
		perror("window_refresh_pro");

	printf("Ctrl+C を押すと終了します\n");
    
	while (1){
		sleep(1);
	}

	for (int i = 0; i < NUM_WINDOWS; i++) {
		remove_window_from(DISPLAY, i);
	}
	disp_release_pro();
exit2:
	screen_exit();
exit:
	munmap(img_va, img_size);
	close(dmabuf_fd);

	return 0;
}
