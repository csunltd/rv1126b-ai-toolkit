//=====================  C++  =====================
#include <string>
#include <list>
//=====================   C   =====================
#include "system.h"
//=====================  PRJ  =====================
#include "system_opt.h"
#include "log_manager.h"
#include "rga_wrapper.h"
#include "display.h"

#include "analyzer.h"

using namespace cv;


static Scalar colorArray[10]={
    Scalar(0, 0, 255, 255),
    Scalar(0, 255, 0, 255),
    Scalar(139,0,0,255),
    Scalar(0,100,0,255),
    Scalar(0,139,139,255),
    Scalar(0,206,209,255),
    Scalar(255,127,0,255),
    Scalar(72,61,139,255),
    Scalar(0,255,0,255),
    Scalar(0,0,255,255),
};
static int plot_one_box(Mat src, int x1, int x2, int y1, int y2, char *label, char colour)
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
static void paint_algorithm_result(Mat image, ChnResult_t result)
{
    // アルゴリズム結果を画像上に描画します
    char text[256];
    for (int algoIndex = 0; algoIndex < ALGOMAXNUM; algoIndex++){
        for (int j = 0; j < result.algoRes[algoIndex].resNumber; j++) {
            detect_result_t *det_result = &(result.algoRes[algoIndex].detect_Group.results[j]);
            if( det_result->prop < 0.4) {
                continue;
            }
            
            // 認識対象のボックスを描画します
            sprintf(text, "%s %.1f%%", det_result->name, det_result->prop * 100);
#if 0
            printf("%s @ (%d %d %d %d) %f\n", det_result->name, det_result->box.left, det_result->box.top,
                   det_result->box.right, det_result->box.bottom, det_result->prop);
#endif
            int x1 = det_result->box.left;
            int y1 = det_result->box.top;
            int x2 = det_result->box.right;
            int y2 = det_result->box.bottom;
            // 認識対象の位置マーカーを描画します
            plot_one_box(image, x1, x2, y1, y2, text, j%10);
        }
    }
}


class Analyzer
{
public:
	Analyzer(int32_t maxChn);
	~Analyzer();

    static Analyzer *instance() { return m_pSelf; }
    static void createAnalyzer(int32_t maxChn);
    
    // --映像リソース処理
    // 1.指定した［映像］チャンネルの画像データを更新します
    int32_t upDateVideoChannel(int chnId, char *imgData, ImgDesc_t imgDesc);
    // 2.指定した［映像］チャンネルの画像データアドレスを取得します
    vChnObject *getVideoChnObject(int chnId);
    uint8_t* videoChannelData(vChnObject *pVideoObj, int &width, int &height);
    // 3.指定した［映像］チャンネルの解析結果を取得します
    int32_t videoChannelAnalyRes(int chnId);

    // --音声リソース処理
    // 1.指定した［音声］チャンネルのデータを更新します
    // 2.指定した［音声］チャンネルのデータアドレスを取得します
    // 3.指定した［音声］チャンネルの解析結果を取得します
    

    bool mAnalyzeThreadWorking;
    bool mDisplayThreadWorking;
    pthread_mutex_t mVideoChnLock;
    //pthread_mutex_t mAudioChnLock;
    int32_t mMaxChnNum;
protected:
    vChnObject *createVideoChnObject(int32_t chnId, int32_t imgWidth, int32_t imgHeight);
    int32_t releaseVideoChnObject(vChnObject *pObj);
    int32_t delAllVideoChannel();

    //aChnObject *searchAudioChnObject(int chnId);
    //aChnObject *createAudioChnObject();
    //int32_t releaseAudioChnObject(aChnObject *pObj);
    //int32_t delAllAudioChannel();

    
private:
    static Analyzer *m_pSelf;
    
    // デコーダ出力データ - RGB 形式
	std::list<vChnObject*> m_VideoChannellist;
	//std::list<aChnObject*> m_MediaAudioChannellist;

	pthread_t mAnalyzeTid;
	pthread_t mDisplayTid;
};

static void *imgAnalyze_thread(void *para)
{
    Analyzer *pSelf = (Analyzer *)para;

    int chnId = 0;
    Mat image;
    ChnResult_t result;
    pSelf->mAnalyzeThreadWorking = true;
    while(1){
        if(!pSelf->mAnalyzeThreadWorking){
            msleep(5);
            break;
        }
        
        if(NULL == pSelf){
            msleep(5);
            break;
        }
        
        vChnObject *pVideoObj = pSelf->getVideoChnObject(chnId);
        if(pVideoObj){
            // 解析対象の画像を取り出します
            pthread_rwlock_rdlock(&pVideoObj->imgLock);
            pVideoObj->image.copyTo(image);
            pthread_rwlock_unlock(&pVideoObj->imgLock);

            // この処理は時間がかかるため、pVideoObj->chnResult に代入する際は pVideoObj が存在するか再確認する必要があります
            result = algorithm_process(chnId, image);
        }        
        pVideoObj = pSelf->getVideoChnObject(chnId);
        if(pVideoObj){
            // 実際には、ここで（異なる解像度の）ストリームを切り替える際にアプリケーションがクラッシュする可能性があります
            memcpy(&pVideoObj->chnResult, &result, sizeof(ChnResult_t));
        }
        
        chnId++;
        chnId%=pSelf->mMaxChnNum;
        msleep(20);
    }
    
    pthread_exit(NULL);
}

static void *imgDisplay_thread(void *para)
{
    Analyzer *pSelf = (Analyzer *)para;

    disp_init();
    // --無シグナルチャンネルの表示内容
    bool bShowNoSig = true;
    Mat noSignal_img = imread("./noSignal.jpg", 1);
    // --シグナルありチャンネルの表示設定
    int videoDuration = 30;//秒
    int preTimeStamp = get_timeval_ms();
    int curTimeStamp = preTimeStamp;
    
    int chnId = 0;
    Mat image = Mat(1080/*height*/, 1920/*width*/, CV_8UC3);
    ChnResult_t result;
    pSelf->mDisplayThreadWorking = true;
    while(1){
        if(!pSelf->mDisplayThreadWorking){
            msleep(5);
            break;
        }
        
        if(NULL == pSelf){
            msleep(5);
            break;
        }

        // videoDuration 秒ごとにチャンネルを切り替えます
        curTimeStamp = get_timeval_ms();
        if(videoDuration*1000 <= (curTimeStamp-preTimeStamp)){
            chnId++;
            chnId%=pSelf->mMaxChnNum;

            preTimeStamp = curTimeStamp;
        }
        
        vChnObject *pVideoObj = pSelf->getVideoChnObject(chnId);
        if(pVideoObj){
            Image srcImage, dstImage;
            memset(&srcImage, 0, sizeof(srcImage));
            memset(&dstImage, 0, sizeof(dstImage));
            srcImage.fmt = RK_FORMAT_BGR_888;
            srcImage.width = pVideoObj->image.cols;
            srcImage.height = pVideoObj->image.rows;
            srcImage.hor_stride = pVideoObj->image.cols;
            srcImage.ver_stride = pVideoObj->image.rows;
            srcImage.rotation = HAL_TRANSFORM_ROT_0;
            srcImage.pBuf = (void *)pVideoObj->image.data;
            dstImage.fmt = RK_FORMAT_BGR_888;
            dstImage.width = image.cols;
            dstImage.height = image.rows;
            dstImage.hor_stride = image.cols;
            dstImage.ver_stride = image.rows;
            dstImage.rotation = HAL_TRANSFORM_ROT_0;
            dstImage.pBuf = (void *)image.data;
            pthread_rwlock_rdlock(&pVideoObj->imgLock);
            // RGA を使用して表示対象画像を高速にコピーします
            srcImg_ConvertTo_dstImg(&dstImage, &srcImage);
            // 解析結果を抽出します
            memset(&result, 0, sizeof(ChnResult_t));
            memcpy(&result, &pVideoObj->chnResult, sizeof(ChnResult_t));
            pthread_rwlock_unlock(&pVideoObj->imgLock);
            
            // 解析結果を表示対象画像に描画します
            paint_algorithm_result(image, result);

            window_commit(image.data, image.cols, image.rows, HAL_TRANSFORM_ROT_270);
            bShowNoSig = true;
            
        }else if(bShowNoSig){
            window_commit(noSignal_img.data, noSignal_img.cols, noSignal_img.rows, HAL_TRANSFORM_ROT_270);
            bShowNoSig = false;
        }
        
        msleep(15);
    }

    disp_exit();
    pthread_exit(NULL);
}

Analyzer *Analyzer::m_pSelf = NULL;
Analyzer::Analyzer(int32_t maxChn) :
    mAnalyzeThreadWorking(false),
    mDisplayThreadWorking(false),
    mMaxChnNum(maxChn)
{
    /*チャンネルロックを初期化します*/
    pthread_mutex_init(&mVideoChnLock, NULL);
    //pthread_mutex_init(&mAudioChnLock, NULL);
    
    /*スレッドを作成します*/
    if(0 != CreateJoinThread(imgAnalyze_thread, this, &mAnalyzeTid)){
        return ;
    }
    
    if(0 != CreateJoinThread(imgDisplay_thread, this, &mDisplayTid)){
        return ;
    }
}
Analyzer::~Analyzer()
{
    /*スレッドを回収します*/
    // 1. ストリーム取得スレッドが起動するまで待機します
    int timeOut_ms = 1000; //n(ms) のタイムアウトを設定し、タイムアウトした場合は待機しません
    while(1){
        if(((true == mDisplayThreadWorking)&&(true == mAnalyzeThreadWorking))||(timeOut_ms <= 0)){
            break;
        }
        timeOut_ms--;
        usleep(1000);
    }
    // 2. スレッドを終了し、終了完了を待機します
    mAnalyzeThreadWorking = false;
    // --[解析スレッドの終了を対象機します]--
    while(1) {
        usleep(20*1000);
        int32_t exitCode = pthread_join(mAnalyzeTid, NULL);
        if(0 == exitCode){
            break;
        }else if(0 != exitCode){
            switch (exitCode) {
                case ESRCH:  // スレッド ID が見つかりません
                    PRINT_ERROR("imgAnalyze_thread exit: No thread with the given ID was found.");
                    break;
                case EINVAL: // スレッドは join できないか、すでに他のスレッドが待機しています
                    PRINT_ERROR("imgAnalyze_thread exit: Thread is detached or already being waited on.");
                    break;
                case EDEADLK: // デッドロック - スレッドが自分自身を join しようとしています
                    PRINT_ERROR("imgAnalyze_thread exit: Deadlock detected - thread is trying to join itself.");
                    break;
            }
            continue;
        }
    }
    mDisplayThreadWorking = false;
    // --[表示スレッドの終了を対象機します]--
    while(1) {
        usleep(20*1000);
        int32_t exitCode = pthread_join(mDisplayTid, NULL);
        if(0 == exitCode){
            break;
        }else if(0 != exitCode){
            switch (exitCode) {
                case ESRCH:  // スレッド ID が見つかりません
                    PRINT_ERROR("imgDisplay_thread exit: No thread with the given ID was found.");
                    break;
                case EINVAL: // スレッドは join できないか、すでに他のスレッドが待機しています
                    PRINT_ERROR("imgDisplay_thread exit: Thread is detached or already being waited on.");
                    break;
                case EDEADLK: // デッドロック - スレッドが自分自身を join しようとしています
                    PRINT_ERROR("imgDisplay_thread exit: Deadlock detected - thread is trying to join itself.");
                    break;
            }
            continue;
        }
    }

    /*映像リソースを回収します*/
    delAllVideoChannel();
    pthread_mutex_destroy(&mVideoChnLock);

    /*音声リソースを回収します*/
    //delAllAudioChannel();
    //pthread_mutex_destroy(&mAudioChnLock);
}
void Analyzer::createAnalyzer(int32_t maxChn)
{
    if(m_pSelf == NULL) {
        m_pSelf = new Analyzer(maxChn);
   }
}

int32_t Analyzer::upDateVideoChannel(int chnId, char *imgData, ImgDesc_t imgDesc)
{
    if(chnId < 0)
        return -1;

    pthread_mutex_lock(&mVideoChnLock);
    vChnObject* targetObj = nullptr;
    for (auto it = m_VideoChannellist.begin(); it != m_VideoChannellist.end(); ++it) {
        // 対象オブジェクトが見つかりました
        if ((*it)->chnId == chnId) {
            targetObj = *it;  
            
            // 画像情報が変更されたため、元の画像キャッシュを破棄します
            if((targetObj->image.cols != imgDesc.width)||(targetObj->image.rows != imgDesc.height)){
                if(0 == releaseVideoChnObject(targetObj)){
                    // リンクリストから chnObj を削除します
                    it = m_VideoChannellist.erase(it);
                }else{
                    pthread_mutex_unlock(&mVideoChnLock);
                    return -2;
                }
            }
            
            break;
        }
    }
    
    // ［映像］チャンネルオブジェクトを作成する必要があります
    if (!targetObj) {
        targetObj = createVideoChnObject(chnId, imgDesc.width, imgDesc.height);
        if(!targetObj)
            return -3;
        
        m_VideoChannellist.push_back(targetObj);
    }
    pthread_mutex_unlock(&mVideoChnLock);

    // ［映像］チャンネルの画像データを更新します
    Image srcImage, dstImage;
    memset(&srcImage, 0, sizeof(srcImage));
    memset(&dstImage, 0, sizeof(dstImage));
    
    srcImage.fmt = rgaFmt(imgDesc.fmt);
    srcImage.width = imgDesc.width;
    srcImage.height = imgDesc.height;
    srcImage.hor_stride = imgDesc.horStride;
    srcImage.ver_stride = imgDesc.verStride;
    srcImage.rotation = HAL_TRANSFORM_ROT_0;
    srcImage.pBuf = imgData;
    
    dstImage.fmt = RK_FORMAT_BGR_888;
    dstImage.width = targetObj->image.cols;
    dstImage.height = targetObj->image.rows;
    dstImage.hor_stride = targetObj->image.cols;
    dstImage.ver_stride = targetObj->image.rows;
    dstImage.rotation = HAL_TRANSFORM_ROT_0;
    dstImage.pBuf = (void *)targetObj->image.data;
    
    pthread_rwlock_wrlock(&targetObj->imgLock);
    srcImg_ConvertTo_dstImg(&dstImage, &srcImage);
    pthread_rwlock_unlock(&targetObj->imgLock);
    return 0;
}

vChnObject *Analyzer::getVideoChnObject(int chnId)
{
    if(chnId < 0)
        return NULL;

    vChnObject* targetObj = nullptr;
    pthread_mutex_lock(&mVideoChnLock);
    for (auto it = m_VideoChannellist.begin(); it != m_VideoChannellist.end(); ++it) {
        // 対象オブジェクトが見つかりました
        if ((*it)->chnId == chnId) {
            targetObj = *it;
            break;
        }
    }
    pthread_mutex_unlock(&mVideoChnLock);

    return targetObj;
}


vChnObject *Analyzer::createVideoChnObject(int32_t chnId, int32_t imgWidth, int32_t imgHeight)
{
    // 1. チャンネルオブジェクトを作成します
    vChnObject* newChnObj = new vChnObject;
    if(!newChnObj)
        return NULL;
    
    // 2. 画像データの読み書きロックを初期化します
    pthread_rwlock_init(&newChnObj->imgLock, nullptr);
    
    // 3. 画像キャッシュを作成します
    newChnObj->chnId = chnId;
    newChnObj->image = Mat(imgHeight, imgWidth, CV_8UC3, Scalar(0, 255, 0));
    memset(&newChnObj->chnResult, 0, sizeof(ChnResult_t));
    
    return newChnObj;
}


int32_t Analyzer::releaseVideoChnObject(vChnObject *pObj)
{
    if(NULL == pObj)
        return -1;
    
    // 1. Mat リソースを破棄します（OpenCV が自動管理）
    pthread_rwlock_wrlock(&pObj->imgLock);
    pObj->image.release();
    pthread_rwlock_unlock(&pObj->imgLock);
    
    // 2. 読み書きロックを破棄します
    pthread_rwlock_destroy(&pObj->imgLock);
    
    // 3. チャンネルオブジェクトを破棄します
    delete pObj;
    
    return 0;
}

int32_t Analyzer::delAllVideoChannel()
{
    pthread_mutex_lock(&mVideoChnLock);
    for (auto it = m_VideoChannellist.begin(); it != m_VideoChannellist.end(); ++it) {
        if(0 == releaseVideoChnObject(*it)){
            it = m_VideoChannellist.erase(it);
        }
    }
    pthread_mutex_unlock(&mVideoChnLock);
    return 0;
}


int analyzer_init(int32_t maxChn)
{
    // 画像アナライザーを作成します
    Analyzer::createAnalyzer(maxChn);
    
    // モデル初期化
    algorithm_init();

    return 0;
}

int videoOutHandle(char *imgData, ImgDesc_t imgDesc)
{
    Analyzer *pAnalyzer = Analyzer::instance();

    if(pAnalyzer){
        pAnalyzer->upDateVideoChannel(imgDesc.chnId, imgData, imgDesc);
    }
    
    return 0;
}

