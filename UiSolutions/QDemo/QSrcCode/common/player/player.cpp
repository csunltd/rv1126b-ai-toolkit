/**
 *
 * Copyright 2026 by Guangzhou Easy EAI Technologny Co.,Ltd.
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

#include <opencv2/opencv.hpp>

#include "system_opt.h"
#include "player.h"

using namespace cv;

typedef struct{
    int resNumber;
//    detect_result_group_t detect_Group;
}AlgoRes_t;

typedef struct{
    AlgoRes_t algoRes[1];
}Result_t;

static bool mThreadWorking = false;
static pthread_t mTid;
static Result_t Result;

static Mat algorithm_image;
static pthread_mutex_t img_lock;
// 対象認識スレッド
void *main_thread_entry(void *para)
{
    int ret;
    Result_t *pResult = (Result_t *)para;

    // 人物検出を初期化します
//    rknn_context ctx;
//    person_detect_init(&ctx, "person_detect.model");

    Mat image = cv::imread("./background.jpg", 1);

    // 初期化画面
    bool bScreenInites = false;
    if (0 == disp_init()) {
        bScreenInites = true;

        set_uiLayer_on_top(true);
        set_alpha_blend_mode(1);
        
	window_commit(image.data, image.cols, image.rows, 0/*HAL_TRANSFORM_ROT_90*/);
    }else{
        fprintf(stderr, "画面の初期化に失敗しました\n");
    }

    mThreadWorking = true;
    while(1) {
        if(!mThreadWorking){
            msleep(5);
            break;
        }

        if(algorithm_image.empty()) {
            msleep(5);
            continue;
        }

        pthread_mutex_lock(&img_lock);
        image = algorithm_image.clone();
        pthread_mutex_unlock(&img_lock);
#if 0
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
#endif
        msleep(16);
    }

    /* 人物検出を解放します */
//    person_detect_release(ctx);
    /* 表示リソースを解放します */
    if(bScreenInites){
        screen_exit();
    }
    return NULL;
}

void mainThread_start()
{
    pthread_mutex_init(&img_lock, NULL);
    memset(&Result, 0, sizeof(Result_t));

    CreateNormalThread(main_thread_entry, &Result, &mTid);

    return;
}

void mainThread_stop()
{
    /*スレッドを回収します*/
    // 1. ストリーム取得スレッドが起動するまで待機します
    int timeOut_ms = 1000; //n(ms) のタイムアウトを設定し、タイムアウトした場合は待機しません
    while(1){
        if((true == mThreadWorking)||(timeOut_ms <= 0)){
            break;
        }
        timeOut_ms--;
        usleep(1000);
    }
    // 2. スレッドを終了し、終了完了を待機します
    mThreadWorking = false;
    // --[解析スレッドの終了を対象機します]--
    while(1) {
        usleep(20*1000);
        int32_t exitCode = pthread_join(mTid, NULL);
        if(0 == exitCode){
            break;
        }else if(0 != exitCode){
            switch (exitCode) {
                case ESRCH:  // スレッド ID が見つかりません
                    printf("imgAnalyze_thread exit: No thread with the given ID was found.");
                    break;
                case EINVAL: // スレッドは join できないか、すでに他のスレッドが待機しています
                    printf("imgAnalyze_thread exit: Thread is detached or already being waited on.");
                    break;
                case EDEADLK: // デッドロック - スレッドが自分自身を join しようとしています
                    printf("imgAnalyze_thread exit: Deadlock detected - thread is trying to join itself.");
                    break;
            }
            continue;
        }
    }
    pthread_mutex_destroy(&img_lock);

    return ;
}
