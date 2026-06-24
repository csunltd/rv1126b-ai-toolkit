#ifndef TOOLS_H
#define TOOLS_H
#include "face_detect.h"
#include <opencv2/opencv.hpp>


struct Transform_info
{
    int src_width;          //元画像幅
    int src_height;         //元画像高さ
    int target_width;       //ターゲット幅
    int target_height;      //ターゲット高さ
    int top;                //上部に追加するピクセル数
    int bottom;             //下部に追加するピクセル数
    int left;               //左側に追加するピクセル数
    int right;              //右側に追加するピクセル数
    float ratio;            //缩放比率, src_width/target_width
};


void nms_cpu(std::vector<det>& boxes, 
             float threshold,
             std::vector<det> &filtered_output);
/*
説明：
    NMS 非最大値抑制

入力：
    boxes               処理対象の det データセット
    threashold          しきい値，面積がしきい値より大きい場合、抑制される可能性があります
    filtered_output     出力する det データセット

出力：
    なし

*/

void letter_box(cv::Mat &src, 
                cv::Mat &dst, 
                int width, 
                int height, 
                struct Transform_info *t_info);
/*
説明：
    OpenCV でパディングし、元画像のアスペクト比を変更せずに目的のサイズへスケーリングします

入力：
    src                 元画像
    dst                 戻り値画像
    width               ターゲット幅
    height              ターゲット高さ
    t_info              変換構造の結果を記録します

出力：
    なし
    
*/

#endif