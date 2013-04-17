#ifndef _UTIL_H_
#define _UTIL_H_

#include "global.h"

void update_color(cv::Mat &image, cv::Point2i point, cv::Scalar color);
void swap_color(cv::Mat &image, cv::Point2i first, cv::Point2i second);
bool compare_color(cv::Mat &image, cv::Point2i point, cv::Scalar color);
bool compare_color(cv::Mat &image, cv::Point2i point, cv::Scalar color, int threshold);

#endif
