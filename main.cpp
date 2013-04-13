#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


cv::Size2i CAMERA_SIZE(1024, 768);
cv::Size2i WINDOW_SIZE(800, 600);

const int COLOR_NUM = 3;
cv::Vec3b BACKGROUND(255, 255, 255);
cv::Vec3b SAND(0, 0, 0);
cv::Vec3b COLORS[COLOR_NUM] = {cv::Vec3b(255, 0, 0), cv::Vec3b(0, 255, 255), cv::Vec3b(0, 0, 255)};

const int AMOUNT = 500;
const int CALIB_SIZE = 50;
const int EDGE = 2;
const int LEFT = 5;
const int LIMIT = 1000;
const int RIGHT = 7;
const int SPEED = 50;
const int THRESHOLD = 300;
const int TIME = 50;
const char* WINDOW_NAME = "OpenCV";

int mode = 0, timer = 0, selected = -1;
cv::Mat transform;
cv::Point2i correction[4] = {cv::Point2i(0, 0), cv::Point2i(0, 0), cv::Point2i(0, 0), cv::Point2i(0, 0)}; 
std::vector<cv::Point2i> clicked;

inline void update_color(cv::Mat &image, cv::Point2i point, cv::Vec3b color) {
    for(int i = 0; i < 3; i++)
        image.data[image.step[0] * point.x + image.step[1] * point.y + i] = color[i];
}

inline void swap_color(cv::Mat &image, cv::Point2i first, cv::Point2i second) {
    for(int i = 0; i < 3; i++)
        std::swap(image.data[image.step[0] * first.x + image.step[1] * first.y + i], image.data[image.step[0] * second.x + image.step[1] * second.y + i]);
}

inline bool compare_color(cv::Mat &image, cv::Point2i point, cv::Vec3b color) {
    bool res = true;
    for(int i = 0; i < 3; i++)
        if(image.data[image.step[0] * point.x + image.step[1] * point.y + i] != color[i])
            res = false;
    return res;
}

cv::Mat calc_transform(void) {
    cv::Point2f src[4] = {cv::Point2f(0, 0), cv::Point2f(0, CAMERA_SIZE.height), cv::Point2f(CAMERA_SIZE.width, 0), cv::Point2f(CAMERA_SIZE.width, CAMERA_SIZE.height)};
    cv::Point2f dst[4];
    for(int i = 0; i < 4; i++) {
        int minimum = INT_MAX;
        for(int j = 0; j < (int)clicked.size(); j++) {
            int distance = pow(src[i].x - clicked[j].x, 2) + pow(src[i].y - clicked[j].y, 2);
            if(distance < minimum) {
                minimum = distance;
                dst[i] = clicked[j] + correction[i];
            }
        }
    }
    return cv::getPerspectiveTransform(dst, src);
}

void mouse_callback(int event, int x, int y, int flags)
{
    if(event == CV_EVENT_LBUTTONDOWN) {
        if(mode == 0) {
            clicked.push_back(cv::Point2i(x, y));
            if(clicked.size() == 4) {
                transform = calc_transform();
                mode = 1;
                selected = -1;
            }
        } else if(mode == 1) {
            mode = 2;
        } else if(mode == 2) {
            mode = 3;
            timer = 0;
        } else if(mode == 3) {
            mode = 2;
        }
    } else if(event == CV_EVENT_RBUTTONDOWN) {
        if(mode == 0 || mode == 1) {
            mode = 0;
            clicked.clear();
        } else if(mode == 2) {
            mode = 1;
            selected = -1;
        }
    }
}

int main(void)
{
    int color, score;
    cv::Mat frame, image;

    srand(time(NULL));
    cv::VideoCapture cap(1);
    if(!cap.isOpened())
        return 1;

    cap.set(CV_CAP_PROP_FRAME_WIDTH, CAMERA_SIZE.width);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, CAMERA_SIZE.height);

    cv::namedWindow(WINDOW_NAME, 0);
    cv::setWindowProperty(WINDOW_NAME, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    cv::setMouseCallback(WINDOW_NAME, (CvMouseCallback)(&mouse_callback));

    while(true) {
        if(mode == 0) {
            cap >> frame;
            for(int i = 0; i < clicked.size(); i++)
                cv::circle(frame, clicked[i], 10, cv::Scalar(0, 0, 255, 0), -1);
            cv::rectangle(frame, cv::Point2i(0, 0), cv::Point2i(CALIB_SIZE, CALIB_SIZE), cv::Scalar(0, 0, 255, 0), -1);
            cv::rectangle(frame, cv::Point2i(0, frame.rows - CALIB_SIZE), cv::Point2i(CALIB_SIZE, frame.rows), cv::Scalar(0, 0, 255, 0), -1);
            cv::rectangle(frame, cv::Point2i(frame.cols - CALIB_SIZE, 0), cv::Point2i(frame.cols, CALIB_SIZE), cv::Scalar(0, 0, 255, 0), -1);
            cv::rectangle(frame, cv::Point2i(frame.cols - CALIB_SIZE, frame.rows - CALIB_SIZE), frame.size(), cv::Scalar(0, 0, 255, 0), -1);
            cv::imshow(WINDOW_NAME, frame);
        } else if(mode == 1) {
            cap >> frame;
            cv::warpPerspective(frame, image, transform, frame.size());
            cv::imshow(WINDOW_NAME, image);
        } else if(mode == 2) {
            cap >> frame;
            frame = cv::Scalar(255, 255, 255);
            cv::imshow(WINDOW_NAME, frame);
        } else if(mode == 3) {
            if(timer == 0) {
                cap >> frame;
                cv::warpPerspective(frame, image, transform, frame.size());

                bool color_use[COLOR_NUM] = {false};
                for(int i = 0; i < image.rows; i++) {
                    for(int j = 0; j < image.cols; j++) {
                        bool flag = false;
                        for(int k = 0; k < COLOR_NUM; k++) {
                            int sum = 0;
                            for(int l = 0; l < 3; l++)
                                sum += abs(COLORS[k][l] - image.data[image.step[0] * i + image.step[1] * j + l]);
                            if(sum < THRESHOLD) {
                                flag = true;
                                color_use[k] = true;
                                update_color(image, cv::Point2i(i, j), COLORS[k]);
                                break;
                            }
                        }
                        if(!flag || i < EDGE || i + EDGE > image.rows || j < EDGE || j + EDGE > image.cols)
                            update_color(image, cv::Point2i(i, j), BACKGROUND);
                    }
                }

                color = 0;
                score = -1;
                for(int i = 0; i < COLOR_NUM; i++)
                    if(color_use[i])
                        color++;
                color = std::max(color, 1);
            }

            if(timer < LIMIT) {
                if(timer < TIME) {
                    for(int i = 0; i < AMOUNT; i++) {
                        cv::Point2i sand(rand() % (image.cols - 10) + 5, rand() % image.rows / 4);
                        if(compare_color(image, cv::Point2i(sand.y, sand.x), BACKGROUND))
                            update_color(image, cv::Point2i(sand.y, sand.x), SAND);
                    }
                }

                for(int i = 0; i < image.rows; i++) {
                    for(int j = 0; j < image.cols; j++) {
                        if(compare_color(image, cv::Point2i(i, j), SAND)) {
                            if(rand() % 100 < SPEED) {
                                if(i == image.rows - 1)
                                    update_color(image, cv::Point2i(i, j), BACKGROUND);
                                else if(compare_color(image, cv::Point2i(i + 1, j), BACKGROUND))
                                        swap_color(image, cv::Point2i(i, j), cv::Point2i(i + 1, j));
                            } else {
                                int horizon = j;
                                if(rand() % 2) {
                                    for(int k = 1; k <= rand() % RIGHT + 1 && j + k < image.cols; k++)
                                        if(compare_color(image, cv::Point2i(i, j + k), BACKGROUND))
                                            horizon = j + k;
                                } else {
                                    for(int k = 1; k <= rand() % LEFT + 1 && j - k >= 0; k++)
                                        if(compare_color(image, cv::Point2i(i, j - k), BACKGROUND))
                                            horizon = j - k;
                                }

                                if(horizon != j)
                                    swap_color(image, cv::Point2i(i, j), cv::Point2i(i, horizon));
                            }
                        }
                    }
                }
            } else if(score == -1) {
                score = 0;
                for(int i = 0; i < image.rows; i++)
                    for(int j = 0; j < image.cols; j++)
                        if(compare_color(image, cv::Point2i(i, j), SAND))
                            score++;

                std::stringstream ss;
                ss << "Score: " << score / color;
                cv::putText(image, ss.str(), cv::Point2i(50, 50), cv::FONT_HERSHEY_PLAIN, 2, cv::Vec3b(0, 0, 0), 2);
            }

            cv::imshow(WINDOW_NAME, image);
            timer += 1;
        } else {
            break;
        }

        int key = cv::waitKey(1);
        if(key == 27) {
            break;
        } else if(mode == 1) {
            if(48 <= key && key <= 51) {
                selected = key - 48;
            } else if(selected != -1) {
                if(key == 'w')
                    correction[selected].y += 1;
                else if(key == 'a')
                    correction[selected].x += 1;
                else if(key == 's')
                    correction[selected].y -= 1;
                else if(key == 'd')
                    correction[selected].x -= 1;
                else
                    continue;
                transform = calc_transform();
            }
        }
    }

    return 0;
}
