#ifndef _SAND_H_
#define _SAND_H_

#include "global.h"

class Sand
{
public:
    Sand(cv::Point2i coord, cv::Vec2d accel, cv::Vec2d speed);

    void addAcceleration(cv::Vec2d accel);
    cv::Point2i getPosition(void);
    bool move(cv::Mat &image);

private:
    cv::Point2i coordinate;
    cv::Vec2d velocity, acceleration;
};

#endif
