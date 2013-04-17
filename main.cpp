#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <sstream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

cv::Size2i CAMERA_SIZE(1024, 768);
cv::Size2i WINDOW_SIZE(800, 600);

const int COLOR_NUM = 3;
cv::Scalar BACKGROUND(255, 255, 255);
cv::Scalar SAND(0, 0, 0);
cv::Scalar COLORS[COLOR_NUM] = {cv::Scalar(255, 0, 0), cv::Scalar(0, 255, 255), cv::Scalar(0, 0, 255)};

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
const char* WINDOW_NAME = "OpenCV";

int mode = 0, timer = 0, selected = -1;
cv::Mat transform;
cv::Point2i correction[4] = {cv::Point2i(0, 0), cv::Point2i(0, 0), cv::Point2i(0, 0), cv::Point2i(0, 0)}; 
std::vector<cv::Point2i> clicked;

inline void update_color(cv::Mat &image, cv::Point2i point, cv::Scalar color) {
    for(int i = 0; i < image.channels(); i++)
        image.data[image.step[0] * point.x + image.step[1] * point.y + i] = color[i];
}

inline void swap_color(cv::Mat &image, cv::Point2i first, cv::Point2i second) {
    for(int i = 0; i < image.channels(); i++)
        std::swap(image.data[image.step[0] * first.x + image.step[1] * first.y + i], image.data[image.step[0] * second.x + image.step[1] * second.y + i]);
}

inline bool compare_color(cv::Mat &image, cv::Point2i point, cv::Scalar color) {
    bool res = true;
    for(int i = 0; i < image.channels(); i++)
        if(image.data[image.step[0] * point.x + image.step[1] * point.y + i] != color[i])
            res = false;
    return res;
}

inline bool compare_color(cv::Mat &image, cv::Point2i point, cv::Scalar color, int threshold) {
    int sum = 0;
    for(int i = 0; i < 3; i++)
        sum += std::abs(image.data[image.step[0] * point.x + image.step[1] * point.y + i] - color[i]);
    return sum < threshold;
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
    int color, score, **field[2];
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

                int buffer[2][image.rows][image.cols];
                bool color_use[COLOR_NUM] = {false}, used[image.rows][image.cols];

                memset(used, 0, sizeof(used));
                for(int i = 0; i < 2; i++) {
                    field[i] = (int **)malloc(sizeof(int *) * image.rows);
                    for(int j = 0; j < image.rows; j++) {
                        field[i][j] = (int *)malloc(sizeof(int) * image.cols);
                        memset(field[i][j], 0, sizeof(int) * image.cols);
                    }
                }

                for(int i = 0; i < image.rows; i++) {
                    for(int j = 0; j < image.cols; j++) {
                        if(used[i][j])
                            continue;

                        for(int k = 0; k < COLOR_NUM; k++) {
                            if(compare_color(image, cv::Point2i(i, j), COLORS[k], THRESHOLD)) {
                                cv::Mat sharp(image.rows, image.cols, CV_8UC1, cv::Scalar(0));
                                cv::Point2i move[4] = {cv::Point2i(1, 0), cv::Point2i(-1, 0), cv::Point2i(0, 1), cv::Point2i(0, -1)};
                                std::queue<cv::Point2i> q;

                                int count = 0;
                                q.push(cv::Point2i(i, j));
                                color_use[k] = true;

                                while(!q.empty()) {
                                    cv::Point2i point, current = q.front();
                                    q.pop();
                                    count++;

                                    if(used[current.x][current.y])
                                        continue;
                                    used[current.x][current.y] = true;
                                    update_color(sharp, current, cv::Scalar(255));
                                    update_color(image, current, COLORS[k]);

                                    for(int l = 0; l < 4; l++) {
                                        point = current + move[l];
                                        if(!(0 <= point.x && point.x < image.rows && 0 <= point.y && point.y < image.cols))
                                            continue;
                                        if(used[point.x][point.y])
                                            continue;
                                        if(compare_color(image, point, COLORS[k], THRESHOLD))
                                            q.push(point);
                                    }
                                }

                                cv::Moments moment = cv::moments(sharp);
                                cv::Point2i center(moment.m01 / moment.m00, moment.m10 / moment.m00);
                                memset(buffer, 0, sizeof(buffer));

                                if(center.x < 0) {
                                    count *= pow(RATIO, -center.x);
                                    center.x = 0;
                                } else if(center.x >= image.rows) {
                                    count *= pow(RATIO, center.x - image.rows + 1);
                                    center.x = image.rows - 1;
                                }
                                if(center.y < 0) {
                                    count *= pow(RATIO, -center.y);
                                    center.y = 0;
                                } else if(center.y >= image.cols) {
                                    count *= pow(RATIO, center.y - image.cols + 1);
                                    center.y = image.cols - 1;
                                }

                                if(center.x > 0)
                                    buffer[0][center.x - 1][center.y] = count;
                                if(center.x < image.rows - 1)
                                    buffer[0][center.x + 1][center.y] = -count;
                                if(center.y > 0)
                                    buffer[1][center.x][center.y - 1] = count;
                                if(center.y < image.cols - 1)
                                    buffer[1][center.x][center.y + 1] = -count;

                                for(int l = center.x - 2; l >= 0; l--)
                                    buffer[0][l][center.y] = buffer[0][l + 1][center.y] * RATIO;
                                for(int l = center.x + 2; l < image.rows; l++)
                                    buffer[0][l][center.y] = buffer[0][l - 1][center.y] * RATIO;
                                for(int l = 0; l < image.rows; l++) {
                                    for(int m = center.y - 1; m >= 0; m--)
                                        buffer[0][l][m] = buffer[0][l][m + 1] * RATIO;
                                    for(int m = center.y + 1; m < image.cols; m++)
                                        buffer[0][l][m] = buffer[0][l][m - 1] * RATIO;
                                }

                                for(int l = center.y - 2; l >= 0; l--)
                                    buffer[1][center.x][l] = buffer[1][center.x][l + 1] * RATIO;
                                for(int l = center.y + 2; l < image.cols; l++)
                                    buffer[1][center.x][l] = buffer[1][center.x][l - 1] * RATIO;
                                for(int l = 0; l < image.cols; l++) {
                                    for(int m = center.x - 1; m >= 0; m--)
                                        buffer[1][m][l] = buffer[1][m + 1][l] * RATIO;
                                    for(int m = center.x + 1; m < image.rows; m++)
                                        buffer[1][m][l] = buffer[1][m - 1][l] * RATIO;
                                }

                                for(int l = 0; l < 2; l++)
                                    for(int m = 0; m < image.rows; m++)
                                        for(int n = 0; n < image.cols; n++)
                                            field[l][m][n] += buffer[l][m][n];

                                break;
                            }
                        }

                        if(!used[i][j] || i < EDGE || i + EDGE > image.rows || j < EDGE || j + EDGE > image.cols)
                            update_color(image, cv::Point2i(i, j), BACKGROUND);
                    }
                }

#ifdef DEBUG
                int maximum[2] = {INT_MIN};
                for(int i = 0; i < image.rows; i++)
                    for(int j = 0; j < image.cols; j++)
                        for(int k = 0; k < 2; k++)
                            maximum[k] = std::max(maximum[k], std::abs(field[k][i][j]));

                for(int i = 0; i < image.rows; i++)
                    for(int j = 0; j < image.cols; j++)
                        update_color(image, cv::Point2i(i, j), cv::Scalar(std::abs(field[0][i][j]) * 255 / maximum[0], std::abs(field[1][i][j]) * 255 / maximum[1], 0));

                cv::imshow(WINDOW_NAME, image);
                cv::waitKey(0);
#endif

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
                        cv::Point2i sand(rand() % 20 + SAND_POSITION - 10, rand() % 20);
                        if(compare_color(image, cv::Point2i(sand.y, sand.x), BACKGROUND))
                            update_color(image, cv::Point2i(sand.y, sand.x), SAND);
                    }
                }

                for(int i = 0; i < image.rows; i++) {
                    for(int j = 0; j < image.cols; j++) {
                        if(compare_color(image, cv::Point2i(i, j), SAND)) {
                            if(rand() % 100 < SPEED) {
                                int vertical = i;
                                for(int k = 1; k <= std::max(field[0][i][j], 1) && i + k < image.rows; k++) {
                                    if(!compare_color(image, cv::Point2i(i + k, j), BACKGROUND))
                                        break;
                                    vertical = i + k;
                                }

                                if(vertical >= image.rows - 1)
                                    update_color(image, cv::Point2i(i, j), BACKGROUND);
                                else if(vertical != i)
                                    swap_color(image, cv::Point2i(i, j), cv::Point2i(vertical, j));
                            } else {
                                int horizon = j;
                                if(rand() % (RIGHT + LEFT) < RIGHT) {
                                    for(int k = 1; k <= std::max(field[1][i][j], 1) && j + k < image.cols; k++) {
                                        if(!compare_color(image, cv::Point2i(i, j + k), BACKGROUND))
                                            break;
                                        horizon = j + k;
                                    }
                                } else {
                                    for(int k = 1; k <= std::max(field[1][i][j], 1) && j - k >= 0; k++) {
                                        if(!compare_color(image, cv::Point2i(i, j - k), BACKGROUND))
                                            break;
                                        horizon = j - k;
                                    }
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

                for(int i = 0; i < 2; i++) {
                    for(int j = 0; j < image.rows; j++)
                        free(field[i][j]);
                    free(field[i]);
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
