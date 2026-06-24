#include <opencv2/opencv.hpp>

#include <sys/time.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "font_engine.h"

int main(void)
{
	// フォントの透明度と色を初期化します
	FontColor color = {200, 135, 189, 67};    // {A, R, G, B};
	
	// グローバルフォントを作成します
	global_font_create("./simhei.ttf", CODE_UTF8);
	
	// ベース画像を読み込みます
	cv::Mat img = cv::imread("./testPic.png");
	
	// 文字を書き込みます
	global_font_set_fontSize(80);
	putText(img.data, img.cols, img.rows, "ようこそ", 210, 940, color);
	global_font_set_fontSize(40);
	putText(img.data, img.cols, img.rows, "ようこそ", 290, 1020, color);
	
	// 文字を書き込んだベース画像を保存します
	cv::imwrite("./result.jpg", img);
	
	// グローバルフォントを破棄します
	global_font_destory();
	
	return 0;
}
