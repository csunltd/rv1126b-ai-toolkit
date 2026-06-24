#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "display.h"

/* 固定の BGR テスト画像を読み込みます。成功時は割り当て済みバッファを返し、失敗時は NULL を返します */
static char *load_test_image(size_t *out_size)
{
#define IMAGE_PATH "720X1280.bgr"
#define IMGRATIO   3
#define IMAGE_SIZE (720 * 1280 * IMGRATIO)
    char *pbuf = (char *)malloc(IMAGE_SIZE);
    if (!pbuf) {
        printf("malloc error: %s, %d\n", __func__, __LINE__);
        return NULL;
    }

    FILE *fp = fopen(IMAGE_PATH, "r");
    if (!fp) {
        printf("fopen error: %s, %d\n", __func__, __LINE__);
        free(pbuf);
        return NULL;
    }

    int ret = fread(pbuf, 1, IMAGE_SIZE, fp);
    fclose(fp);
    if (ret != IMAGE_SIZE) {
        printf("fread error: %s, %d\n", __func__, __LINE__);
        free(pbuf);
        return NULL;
    }

    if (out_size) {
        *out_size = IMAGE_SIZE;
    }
    return pbuf;
}

int main(int argc, char *argv[])
{
    if(disp_init()){
        fprintf(stderr, "display 領域の初期化に失敗しました\n");
        return -2;        
    }
    printf("display 領域の初期化に成功しました。BGR テスト画像を表示しています...\n");
    
    // 画像データを読み込みます
    size_t image_size = 0;
    char *pbuf = load_test_image(&image_size);
    if (!pbuf) {
        return -1;
    }

    // 画像をウィンドウ内に表示します［引き伸ばしスケーリングを使用］
    window_commit(pbuf, 720, 1280, 0);
    printf("Ctrl+C を押すと終了します\n");
    
    // 表示を維持します
    while (1) {
        // ここで画像内容を更新することもできます
        sleep(1);
    }

    free(pbuf);
    // disp_release(); //disp_exit();にはすでに含まれていますdisp_release();処理

    disp_exit();
    
    return 0;
}
