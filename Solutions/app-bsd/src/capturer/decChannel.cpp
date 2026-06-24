//=====================  C++  =====================
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
//=====================   C   =====================
#include <gst/video/video-info.h>
#include "system.h"
//=====================  SDK  =====================
#include "system_opt.h"
#include "gst_opt.h"
//=====================  PRJ  =====================
#include "decChannel.h"

#include "../analyzer/analyzer.h"


#define CAMERA_WIDTH    1920
#define CAMERA_HEIGHT   1080
#define	IMGRATIO        3
#define	IMAGE_SIZE      (CAMERA_WIDTH*CAMERA_HEIGHT*IMGRATIO)
// =======================================================================================================
void *getFrameThread(void *para)
{
    int ret;
    GstChannel_t *pChannel = (GstChannel_t *)para;

    pChannel->bGetFrameRunning = true;
    while(pChannel->bGetFrameRunning) {
        SrcFrame_t srcFrame = {0};
        ret = mipicamera_getSrcframe(pChannel->cameraIndex, &srcFrame);
        if (ret) {
            /* DQBUF が失敗した場合は QBUF してはいけません。キュー破損またはクラッシュの原因になります */
            printf("error: %s, %d\n", __func__, __LINE__);
            continue;
        }

        ImgDesc_t imgDesc = {0};
        imgDesc.chnId = pChannel->chnId;
        imgDesc.width = srcFrame.width;
        imgDesc.height = srcFrame.height;
        imgDesc.horStride = srcFrame.horStride;
        imgDesc.verStride = srcFrame.verStride;
        imgDesc.dataSize = srcFrame.dataSize;
        strcpy(imgDesc.fmt, "NV12");
        videoOutHandle((char *)srcFrame.pMapBuff, imgDesc);

        
        mipicamera_putSrcframe(pChannel->cameraIndex, &srcFrame);
        usleep(10*1000);
    }
    mipicamera_exit(pChannel->cameraIndex);
    
    pthread_exit(NULL);
}

DecChannel::DecChannel(int chnId, std::string strUrl, std::string strVedioFmt) :
    mTid(0),
	bObjIsInited(false),
    mStrUrl(strUrl),
    mStrVideoFmt(strVedioFmt)
{
    memset(&mGstChn, 0, sizeof(mGstChn));
    mGstChn.chnId = chnId;
}
DecChannel::~DecChannel()
{
    /*スレッドを回収します*/
    // 1. ストリーム取得スレッドが起動するまで待機します
    int timeOut_ms = 1500; //n(ms) のタイムアウトを設定し、タイムアウトした場合は待機しません
    while(1){
        if((true == mGstChn.bGetFrameRunning)||(timeOut_ms <= 0)){
            break;
        }
        timeOut_ms--;
        usleep(1000);
    }
    // 2. スレッドを終了し、終了完了を待機します
    if(mTid){
        mGstChn.bGetFrameRunning = false;
        // --[ストリーム取得スレッドの終了を対象機します]--
        while(1) {
            usleep(20*1000);
            int32_t exitCode = pthread_join(mTid, NULL);
            if(0 == exitCode){
                break;
            }else if(0 != exitCode){
                switch (exitCode) {
                    case ESRCH:  // スレッド ID が見つかりません
                        printf("getFrameThread exit: No thread with the given ID was found.\n");
                        break;
                    case EINVAL: // スレッドは join できないか、すでに他のスレッドが待機しています
                        printf("getFrameThread exit: Thread is detached or already being waited on.\n");
                        break;
                    case EDEADLK: // デッドロック - スレッドが自分自身を join しようとしています
                        printf("getFrameThread exit: Deadlock detected - thread is trying to join itself.\n");
                        break;
                }
                continue;
            }
        }
    }

    if(bObjIsInited){
        bObjIsInited = false;
    }
}
int DecChannel::init()
{
    mGstChn.cameraIndex = atoi(mStrUrl.c_str()); //通常、csi0 は video23、csi1 は video31 です
    
    // 1.カメラを開きます
    int ret = mipicamera_init(mGstChn.cameraIndex, CAMERA_WIDTH, CAMERA_HEIGHT, 0);
    if (ret) {
        printf("error: %s, %d\n", __func__, __LINE__);
        return -1;
    }
    
    // 2.先頭 10 フレームをスキップします
    char *pbuf = (char *)malloc(IMAGE_SIZE);
    if(pbuf){
        int skip = 10;
        while(skip--) {
            ret = mipicamera_getframe(mGstChn.cameraIndex, pbuf);
            if (ret) {
                printf("error: %s, %d\n", __func__, __LINE__);
                mipicamera_exit(mGstChn.cameraIndex);
                return -1;
            }
        }
        free(pbuf);
    }

	if(0 != CreateJoinThread(getFrameThread, &mGstChn, &mTid)){
        mipicamera_exit(mGstChn.cameraIndex);
        return -1;
    }
    bObjIsInited = true;
    return 0;
}
