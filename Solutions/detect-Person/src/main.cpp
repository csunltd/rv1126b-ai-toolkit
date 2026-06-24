#include <stdio.h>
#include <unistd.h>

#include "system_opt.h"
#include "person_detect.h"
#include "display.h"

#include "camera/camera.h"



using namespace std;
using namespace cv;
static Scalar colorArray[10]={
    Scalar(255, 0, 0, 255),
    Scalar(0, 255, 0, 255),
    Scalar(0,0,139,255),
    Scalar(0,100,0,255),
    Scalar(139,139,0,255),
    Scalar(209,206,0,255),
    Scalar(0,127,255,255),
    Scalar(139,61,72,255),
    Scalar(0,255,0,255),
    Scalar(255,0,0,255),
};

typedef struct{
    int resNumber;
	detect_result_group_t detect_Group;    
}AlgoRes_t;

typedef struct{
	AlgoRes_t algoRes[1];
}Result_t;

Mat algorithm_image;
pthread_mutex_t img_lock;

int plot_one_box(Mat src, int x1, int x2, int y1, int y2, char *label, char colour)
{
    int tl = round(0.002 * (src.rows + src.cols) / 2) + 1;
    rectangle(src, cv::Point(x1, y1), cv::Point(x2, y2), colorArray[(unsigned char)colour], 3);

    int tf = max(tl -1, 1);

    int base_line = 0;
    cv::Size t_size = getTextSize(label, FONT_HERSHEY_SIMPLEX, (float)tl/3, tf, &base_line);
    int x3 = x1 + t_size.width;
    int y3 = y1 - t_size.height - 3;

    rectangle(src, cv::Point(x1, y1), cv::Point(x3, y3), colorArray[(unsigned char)colour], -1);
    putText(src, label, cv::Point(x1, y1 - 2), FONT_HERSHEY_SIMPLEX, (float)tl/3, cv::Scalar(255, 255, 255, 255), tf, 8);
    return 0;
}

// 対象認識スレッド
void *detect_thread_entry(void *para)
{
    int ret;
    Result_t *pResult = (Result_t *)para;
	
    // 人物検出を初期化します
    rknn_context ctx;
    person_detect_init(&ctx, "person_detect.model");
	
    Mat image;
    while(1)
    {
        if(algorithm_image.empty()) {
            usleep(5);
            continue;
        }
		
        pthread_mutex_lock(&img_lock);
        image = algorithm_image.clone();
        pthread_mutex_unlock(&img_lock);
        // アルゴリズム解析
        ret = person_detect_run(ctx, image, &pResult->algoRes[0].detect_Group);
        if(0 != ret){
            usleep(1000);
            continue;
        }

        pResult->algoRes[0].resNumber = pResult->algoRes[0].detect_Group.count;
        if(pResult->algoRes[0].resNumber <= 0){
            memset(&pResult->algoRes[0], 0, sizeof(AlgoRes_t));
            usleep(1000);
            continue;
        }

        printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
        printf("person number : %d\n", pResult->algoRes[0].resNumber);
		
        usleep(16*1000);
    }
    /* 人物検出を解放します */
    person_detect_release(ctx);
    return NULL;
}

int main(int argc, char **argv)
{
    int ret = 0;
    if(1 == argc){
        printf("\nerr: Missing parameter!\n");
        printf("================= [usage] ==================\n");
        printf("example:\n");
        printf("\t%s <23/31>\n", argv[0]);
        printf("--------------------------------------------\n");
        return 0;
    }

    // 対象認識スレッド関連の変数
    pthread_t mTid;
    Result_t Result;

    // camera の元画像関連の変数
    int skip = 0;
    char *pbuf = NULL;
    Mat image;

    // 1.ディスプレイを初期化します
    if (0 != disp_init()) {
        fprintf(stderr, "DRM の初期化に失敗しました\n");
        return -1;
    }

    char timestamp[64] = {0};

    // 2.カメラを開きます
#define CAMERA_WIDTH    1920
#define CAMERA_HEIGHT   1080
#define	IMGRATIO        3
#define	IMAGE_SIZE      (CAMERA_WIDTH*CAMERA_HEIGHT*IMGRATIO)
    int cameraIndex = atoi(argv[1]); //通常、csi0 は video23、csi1 は video31 です
    ret = mipicamera_init(cameraIndex, CAMERA_WIDTH, CAMERA_HEIGHT, 0);
    if (ret) {
        printf("error: %s, %d\n", __func__, __LINE__);
        goto exit3;
    }
    
    pbuf = (char *)malloc(IMAGE_SIZE);
    //pbuf = dma_alloc(&dma_fd, IMAGE_SIZE);
    if (!pbuf) {
        printf("error: %s, %d\n", __func__, __LINE__);
        ret = -1;
        goto exit2;
    }
    
    //先頭 10 フレームをスキップします
    skip = 10;
    while(skip--) {
        ret = mipicamera_getframe(cameraIndex, pbuf);
        if (ret) {
            printf("error: %s, %d\n", __func__, __LINE__);
            goto exit1;
        }
    }
	
    // 3.認識スレッドおよび画像ミューテックスを作成します
    pthread_mutex_init(&img_lock, NULL);
    memset(&Result, 0, sizeof(Result_t));
    CreateNormalThread(detect_thread_entry, &Result, &mTid);

    // 4.（ストリーム取得 + 表示）ループ
    while(1) {
        // 4.1、ストリーム取得
        pthread_mutex_lock(&img_lock);
        ret = mipicamera_getframe(cameraIndex, pbuf);
        if(ret) {
            printf("error: %s, %d\n", __func__, __LINE__);
            pthread_mutex_unlock(&img_lock);
            continue;
        }
        algorithm_image = Mat(CAMERA_HEIGHT, CAMERA_WIDTH, CV_8UC3, pbuf);
        image = algorithm_image.clone();
        pthread_mutex_unlock(&img_lock);

        // 4.2、表示
        char text[256];
        for (int i = 0; i < Result.algoRes[0].resNumber; i++) {
            detect_result_t *det_result = &(Result.algoRes[0].detect_Group.results[i]);
            if( det_result->prop < 0.4) {
                continue;
            }

           // 認識対象のボックスを描画します
           sprintf(text, "%s %.1f%%", det_result->name, det_result->prop * 100);
           printf("%s @ (%d %d %d %d) %f\n", det_result->name, det_result->box.left, det_result->box.top,
    			   det_result->box.right, det_result->box.bottom, det_result->prop);
            int x1 = det_result->box.left;
            int y1 = det_result->box.top;
            int x2 = det_result->box.right;
            int y2 = det_result->box.bottom;
            // 認識対象の位置マーカーを描画します
            /*
            rectangle(image, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(255, 0, 0, 255), 3);
            putText(image, text, cv::Point(x1, y1 + 12), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
            */
            plot_one_box(image, x1, x2, y1, y2, text, i%10);
        }

        window_commit(image.data, CAMERA_WIDTH, CAMERA_HEIGHT, HAL_TRANSFORM_ROT_270);
        usleep(15*1000); //15ms
    }

exit1:
    //dma_free(&dma_fd, pbuf, IMAGE_SIZE);
    free(pbuf);
    pbuf = NULL;
    
exit2:
    mipicamera_exit(cameraIndex);
    
exit3:
    disp_exit();
    
    return ret;
}

