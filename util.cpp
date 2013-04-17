#include "util.h"

void update_color(cv::Mat &image, cv::Point2i point, cv::Scalar color) {
    for(int i = 0; i < image.channels(); i++)
        image.data[image.step[0] * point.x + image.step[1] * point.y + i] = color[i];
}

void swap_color(cv::Mat &image, cv::Point2i first, cv::Point2i second) {
    for(int i = 0; i < image.channels(); i++)
        std::swap(image.data[image.step[0] * first.x + image.step[1] * first.y + i], image.data[image.step[0] * second.x + image.step[1] * second.y + i]);
}

bool compare_color(cv::Mat &image, cv::Point2i point, cv::Scalar color) {
    bool res = true;
    for(int i = 0; i < image.channels(); i++)
        if(image.data[image.step[0] * point.x + image.step[1] * point.y + i] != color[i])
            res = false;
    return res;
}

bool compare_color(cv::Mat &image, cv::Point2i point, cv::Scalar color, int threshold) {
    int sum = 0;
    for(int i = 0; i < 3; i++)
        sum += std::abs(image.data[image.step[0] * point.x + image.step[1] * point.y + i] - color[i]);
    return sum < threshold;
}
