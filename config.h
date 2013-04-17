#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "global.h"

const cv::Size2i CAMERA_SIZE(1024, 768);
const cv::Size2i WINDOW_SIZE(800, 600);

const int COLOR_NUM = 3;
const cv::Scalar BACKGROUND(255, 255, 255);
const cv::Scalar SAND(0, 0, 0);
const cv::Scalar COLORS[COLOR_NUM] = {cv::Scalar(255, 0, 0), cv::Scalar(0, 255, 255), cv::Scalar(0, 0, 255)};

const int AMOUNT = 300;
const int CALIB_SIZE = 50;
const int EDGE = 2;
const int LEFT = 5;
const int LIMIT = 1000;
const double RATIO = 0.9;
const int RIGHT = 5;
const int SAND_POSITION = 512;
const int SPEED = 50;
const int THRESHOLD = 300;
const int TIME = 200;
const char WINDOW_NAME[7] = "OpenCV";

#endif
