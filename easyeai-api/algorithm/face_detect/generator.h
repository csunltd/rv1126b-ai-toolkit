#ifndef GENERATOR_H
#define GENERATOR_H

#include <string.h>
#include <vector>
#include <iostream>
#include <cmath>
#include<opencv2/opencv.hpp>
#include "generator.h"


using namespace std;


vector<vector<float>> generate_prior_data(int width, int height);
/*
概要：
    生成prior_data。
パラメータ：
    width           画像幅
    height          画像高さ
戻り値：
    2 次元浮動小数点 vector。D1 は個数、D2 は 1 つの prior_data の長さです。
*/


void decode_box(float *loc, 
                const vector<float> &single_prior_data, 
                cv::Rect_<float> &rect);
/*
概要：
    prior_data と予測値に基づいて、単一の顔ボックス情報をデコードします
パラメータ：
    loc                 モデルの loc 予測値セット
    single_prior_data   単一の prior_data（事前データ）
    box                 box 配列。結果はこの中に返されます
戻り値：
    なし
*/



void decode_landmark(float *predict, 
                     const vector<float> &single_prior_data, 
                     vector<cv::Point2f> &landmark);
/*
概要：
    prior_data と予測値に基づいて、単一の顔ランドマーク情報をデコードします。
パラメータ：
    predict             モデルの predict 予測値セット
    single_prior_data   単一の prior_data（事前データ）
    landmark            landmark 配列。結果はこの中に返されます
戻り値：
    なし
*/

#endif




