#ifndef FACE_DETECT_H
#define FACE_DETECT_H

#include<opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include "rknn_api.h"



class det {
public:
    det(){
		landmarks.resize(5);
    }
    ~det(){
    }

    bool operator<(const det &t) const {
		return score < t.score;
	}

    bool operator>(const det &t) const {
		return score > t.score;
	}

    cv::Rect_<float> box;
    std::vector<cv::Point2f> landmarks;
    float score;

    void print() {
		printf("finalbox(x1y1x2y2) %f %f %f %f, score %f\n", box.x, box.y, box.br().x, box.br().y, score);
		printf("landmarks ");
		int point_number = landmarks.size();
		for (int i = 0; i < point_number; i++) {
			printf("%f %f, ", landmarks[i].x, landmarks[i].y);
		}
		printf("\n");
	}
    
};

/* 
 * 顔検出を初期化します関数
 * ctx:入力パラメータ,rknn_contextハンドル
 * path:入力パラメータ,アルゴリズムモデルパス
 */
int face_detect_init(rknn_context *ctx, const char *path);

/* 
 * 顔検出実行関数
 * ctx:入力パラメータ, rknn_contextハンドル
 * input_image:入力パラメータ, Opencv Mat形式画像
 * result：出力パラメータ, 顔検出結果を出力します
 */
int face_detect_run(rknn_context ctx, cv::Mat &input_image, std::vector<det> &result);

/* 
 * 顔検出解放関数
 * ctx:入力パラメータ, rknn_contextハンドル
 */
int face_detect_release(rknn_context ctx);


#endif
