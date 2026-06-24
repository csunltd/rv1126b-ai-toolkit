//=====================  C++  =====================
#include <string>
//=====================   C   =====================
#include "system.h"
//=====================  PRJ  =====================
#include "algoProcess.h"

static bool g_Algorithm_is_NotReady = true;

//static rknn_context gBSDCtx;

int algorithm_init()
{
	//bsd_init(&gBSDCtx, "bsd_person.model");

    g_Algorithm_is_NotReady = false;
    return 0;
}

int algorithm_unInit()
{
	//bsd_release(gBSDCtx);
	return 0;
}

ChnResult_t algorithm_process(int chnId, Mat image)
{
    int ret = 0;
    ChnResult_t chnResult={0};
    
    int resultNum = 0;
    detect_result_group_t detect_result_group = {0};

    // モデルの読み込みが完了していないため、オブジェクト検出などの処理は実行しません
    if(g_Algorithm_is_NotReady){
		usleep(1000);
		return chnResult;
    }
    // オブジェクト検出を開始します:
    // ==========================================================================================
	//ret = bsd_run(gBSDCtx, image, &detect_result_group);
    //if(0 != ret){
	//	usleep(1000);
	//	return chnResult;
    //}
    
	resultNum = detect_result_group.count;
	if(resultNum <= 0){
        memset(&chnResult.algoRes[0], 0, sizeof(AlgoRes_t));
		usleep(1000);
		return chnResult;
	}
#if 0
	printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    printf("===[channel]--[%d]:\n", chnId);
	printf("============person number : %d\n", resultNum);
#endif
    chnResult.algoRes[0].resNumber = resultNum;
    memcpy(&chnResult.algoRes[0].detect_Group, &detect_result_group, sizeof(detect_result_group));

    return chnResult;
}

