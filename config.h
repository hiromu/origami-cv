#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "global.h"

const cv::Size2i CAMERA_SIZE(1024, 768);
const cv::Size2i WINDOW_SIZE(800, 600);

const int COLOR_NUM = 2;
const cv::Scalar BACKGROUND(255, 255, 255);
const cv::Scalar SAND(0, 0, 0);
const cv::Scalar COLORS[COLOR_NUM] = {cv::Scalar(255, 0, 0), cv::Scalar(0, 0, 255)};

const int ACCEL = 20;
const int AMOUNT = 300;
const int BOX_DIFF = 100;
const int BOX_SIZE = 100;
const int CALIB_SIZE = 20;
const int EDGE = 2;
const int EDGE_FIELD = 20;
const double EFFECT = 0.05;
const int LEFT = 1;
const int LIMIT = 1000;
const int MOVE_CHECK = 50;
const double RATIO = 0.995;
const int RIGHT = 1;
const int SPEED = 10;
const int THRESHOLD = 300;
const int TIME = 200;
const char WINDOW_NAME[7] = "OpenCV";

#endif
