#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "person_pose.h"
#include <opencv2/opencv.hpp>


// 線を描画します
cv::Mat draw_line(cv::Mat image, float *key1, float *key2, cv::Scalar color)
{
	if (key1[2] > 0.1 && key2[2] > 0.1) {
		cv::Point pt1(key1[0], key1[1]);
		cv::Point pt2(key2[0], key2[1]);
		cv::circle(image, pt1, 2, color, 2);
		cv::circle(image, pt2, 2, color, 2);
		cv::line(image, pt1, pt2, color, 2);
	}

	return image;
}


// 結果を描画します:
// 0 鼻、1 左目、2 右目、3 左耳、4 右耳、5 左肩、6 右肩、7 左肘、8 右肘、9 左手首、10 右手首、11 左股関節、12 右股関節、13 左膝、14 右膝、15 左足首、16 右足首
cv::Mat draw_image(cv::Mat image, std::vector<person_pose_result_t> results)
{
	long unsigned int i =0;
	for (i = 0; i < results.size(); i++) {
		// 顔部分を描画します
		image = draw_line(image, results[i].keypoints[0], results[i].keypoints[1], CV_RGB(0, 255, 0));
		image = draw_line(image, results[i].keypoints[0], results[i].keypoints[2], CV_RGB(0, 255, 0));
		image = draw_line(image, results[i].keypoints[1], results[i].keypoints[3], CV_RGB(0, 255, 0));
		image = draw_line(image, results[i].keypoints[2], results[i].keypoints[4], CV_RGB(0, 255, 0));
		image = draw_line(image, results[i].keypoints[3], results[i].keypoints[5], CV_RGB(0, 255, 0));
		image = draw_line(image, results[i].keypoints[4], results[i].keypoints[6], CV_RGB(0, 255, 0));

		// 上半身を描画します
		image = draw_line(image, results[i].keypoints[5], results[i].keypoints[6], CV_RGB(0, 0, 255));
		image = draw_line(image, results[i].keypoints[5], results[i].keypoints[7], CV_RGB(0, 0, 255));
		image = draw_line(image, results[i].keypoints[7], results[i].keypoints[9], CV_RGB(0, 0, 255));
		image = draw_line(image, results[i].keypoints[6], results[i].keypoints[8], CV_RGB(0, 0, 255));
		image = draw_line(image, results[i].keypoints[8], results[i].keypoints[10], CV_RGB(0, 0, 255));
		image = draw_line(image, results[i].keypoints[5], results[i].keypoints[11], CV_RGB(0, 0, 255));
		image = draw_line(image, results[i].keypoints[6], results[i].keypoints[12], CV_RGB(0, 0, 255));
		image = draw_line(image, results[i].keypoints[11], results[i].keypoints[12], CV_RGB(0, 0, 255));

		// 下半身を描画します
		image = draw_line(image, results[i].keypoints[11], results[i].keypoints[13], CV_RGB(255, 255, 0));
		image = draw_line(image, results[i].keypoints[13], results[i].keypoints[15], CV_RGB(255, 255, 0));
		image = draw_line(image, results[i].keypoints[12], results[i].keypoints[14], CV_RGB(255, 255, 0));
		image = draw_line(image, results[i].keypoints[14], results[i].keypoints[16], CV_RGB(255, 255, 0));

		cv::Rect rect(results[i].left, results[i].top, (results[i].right - results[i].left), (results[i].bottom - results[i].top));
		cv::rectangle(image, rect, CV_RGB(255, 0, 0), 2);
	}

	return image;
}

/// メイン関数
int main(int argc, char **argv)
{
    if (argc != 3) {
        printf("%s <model_path> <image_path>\n", argv[0]);
        return -1;
    }


    const char *p_model_path = argv[1];
    const char *p_img_path = argv[2];
	printf("Model path = %s, image path = %s\n\n", p_model_path, p_img_path);
	cv::Mat image = cv::imread(p_img_path);

	printf("Image size = (%d, %d)\n", image.rows, image.cols);
    int ret;
	person_pose_context_t yolo11_pose;
    memset(&yolo11_pose, 0, sizeof(yolo11_pose));

	person_pose_init(p_model_path, &yolo11_pose, 1);


	double start_time = static_cast<double>(cv::getTickCount());
	std::vector<person_pose_result_t> results = person_pose_run(image, &yolo11_pose, 0.35, 0.35);

	double end_time = static_cast<double>(cv::getTickCount());
	double time_elapsed = (end_time - start_time) / cv::getTickFrequency() * 1000;
	std::cout << "person pose run time: " << time_elapsed << " ms" << std::endl;

	// 結果を描画します
	image = draw_image(image, results);

	cv::imwrite("result.jpg", image);
	printf("Detect size = %ld\n", results.size());

    ret = person_pose_release(&yolo11_pose);

    return ret;
}