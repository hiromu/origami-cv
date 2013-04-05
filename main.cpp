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

const int COLOR_NUM = 6;
cv::Vec3b BACKGROUND(255, 255, 255);
cv::Vec3b SAND(0, 0, 0);
cv::Vec3b COLORS[COLOR_NUM] = {cv::Vec3b(255, 0, 0), cv::Vec3b(0, 255, 0), cv::Vec3b(0, 0, 255), cv::Vec3b(255, 255, 0), cv::Vec3b(255, 0, 255), cv::Vec3b(0, 255, 255)};

const int AMOUNT = 500;
const int CALIB_SIZE = 50;
const int LEFT = 5;
const int LIMIT = 1000;
const int RIGHT = 7;
const int SPEED = 50;
const int THRESHOLD = 150;
const int TIME = 50;
const char* WINDOW_NAME = "OpenCV";

int mode = 0, timer = 0;
cv::Mat transform;
std::vector<cv::Point2i> clicked;

void mouse_callback(int event, int x, int y, int flags)
{
    if(event == CV_EVENT_LBUTTONDOWN) {
        if(mode == 0) {
            clicked.push_back(cv::Point2i(x, y));
            if(clicked.size() == 4) {
                cv::Point2f src[4] = {cv::Point2f(0, 0), cv::Point2f(0, CAMERA_SIZE.height), cv::Point2f(CAMERA_SIZE.width, 0), cv::Point2f(CAMERA_SIZE.width, CAMERA_SIZE.height)};
                cv::Point2f dst[4];
                for(int i = 0; i < 4; i++) {
                    int minimum = INT_MAX;
                    for(int j = 0; j < (int)clicked.size(); j++) {
                        int distance = pow(src[i].x - clicked[j].x, 2) + pow(src[i].y - clicked[j].y, 2);
                        if(distance < minimum) {
                            minimum = distance;
                            dst[i] = clicked[j];
                        }
                    }
                }
                mode = 1;
                transform = cv::getPerspectiveTransform(dst, src);
                for(int i = 0; i < transform.cols; i++) {
                    for(int j = 0; j < transform.rows; j++)
                        std::cout << transform.at<int>(i, j) << " ";
                    std::cout << std::endl;
                }
            }
        } else if(mode == 1) {
            mode = 2;
            timer = 0;
        } else if(mode == 2) {
            mode = 1;
        }
    } else if(event == CV_EVENT_RBUTTONDOWN) {
        if(mode == 0 || mode == 1) {
            mode = 0;
            clicked.clear();
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
                                for(int l = 0; l < 3; l++)
                                    image.data[image.step[0] * i + image.step[1] * j + l] = COLORS[k][l];
                                break;
                            }
                        }
                        if(!flag)
                            for(int k = 0; k < 3; k++)
                                image.data[image.step[0] * i + image.step[1] * j + k] = BACKGROUND[k];
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

                        bool flag = true;
                        for(int j = 0; j < 3; j++) {
                            if(image.data[image.step[0] * sand.y + image.step[1] * sand.x + j] != BACKGROUND[j]) {
                                flag = false;
                                break;
                            }
                        }

                        if(flag)
                            for(int j = 0; j < 3; j++)
                                image.data[image.step[0] * sand.y + image.step[1] * sand.x + j] = SAND[j];
                    }
                }

                for(int i = 0; i < image.rows; i++) {
                    for(int j = 0; j < image.cols; j++) {
                        bool flag = true;
                        for(int k = 0; k < 3; k++) {
                            if(image.data[image.step[0] * i + image.step[1] * j + k] != SAND[k]) {
                                flag = false;
                                break;
                            }
                        }

                        if(flag) {
                            if(rand() % 100 < SPEED) {
                                if(i == image.rows - 1) {
                                    for(int k = 0; k < 3; k++)
                                        image.data[image.step[0] * i + image.step[1] * j + k] = BACKGROUND[k];
                                } else {
                                    flag = true;
                                    for(int k = 0; k < 3; k++) {
                                        if(image.data[image.step[0] * (i + 1) + image.step[1] * j + k] != BACKGROUND[k]) {
                                            flag = false;
                                            break;
                                        }
                                    }

                                    if(flag) {
                                        for(int k = 0; k < 3; k++) { 
                                            image.data[image.step[0] * i + image.step[1] * j + k] = image.data[image.step[0] * (i + 1) + image.step[1] * j + k];
                                            image.data[image.step[0] * (i + 1) + image.step[1] * j + k] = SAND[k];
                                        }
                                    }
                                }
                            } else {
                                int horizon = j;
                                if(rand() % 2) {
                                    for(int k = 1; k <= rand() % RIGHT + 1 && j + k < image.cols; k++) {
                                        flag = true;
                                        for(int l = 0; l < 3; l++) {
                                            if(image.data[image.step[0] * i + image.step[1] * (j + k) + l] != BACKGROUND[l]) {
                                                flag = false;
                                                break;
                                            }
                                        }
                                        if(flag)
                                            horizon = j + k;
                                    }
                                } else {
                                    for(int k = 1; k <= rand() % LEFT + 1 && j - k >= 0; k++) {
                                        flag = true;
                                        for(int l = 0; l < 3; l++) {
                                            if(image.data[image.step[0] * i + image.step[1] * (j - k) + l] != BACKGROUND[l]) {
                                                flag = false;
                                                break;
                                            }
                                        }
                                        if(flag)
                                            horizon = j - k;
                                    }
                                }

                                if(horizon != j) {
                                    for(int k = 0; k < 3; k++) { 
                                        image.data[image.step[0] * i + image.step[1] * j + k] = image.data[image.step[0] * i + image.step[1] * horizon + k];
                                        image.data[image.step[0] * i + image.step[1] * horizon + k] = SAND[k];
                                    }
                                }
                            }
                        }
                    }
                }
            } else if(score == -1) {
                score = 0;
                for(int i = 0; i < image.rows; i++) {
                    for(int j = 0; j < image.cols; j++) {
                        bool flag = true;
                        for(int k = 0; k < 3; k++) {
                            if(image.data[image.step[0] * i + image.step[1] * j + k] != SAND[k]) {
                                flag = false;
                                break;
                            }
                        }
                        if(flag)
                            score++;
                    }
                }

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
        if(key == 27)
            break;
    }

    return 0;
}
