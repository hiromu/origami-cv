#ifndef _MAIN_H_
#define _MAIN_H_

#include "global.h"
#include "sand.h"

int mode = 0, timer = 0, selected = -1, center = -1, box = -1;
cv::Mat transform;
cv::Point2i correction[4] = {cv::Point2i(0, 0), cv::Point2i(0, 0), cv::Point2i(0, 0), cv::Point2i(0, 0)}; 
std::vector<cv::Point2i> clicked;
std::vector<Sand> sands;

cv::Mat calc_transform(void);
void mouse_callback(int event, int x, int y, int flags);

#endif
