#include "main.h"

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
            box = -1;
            center = -1;
            mode = 2;
        } else if(mode == 2) {
            mode = 3;
            timer = 0;
        } else if(mode == 3) {
            box = -1;
            center = -1;
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
    double **imos;
    int color = 0, score = 0;
    cv::Mat frame, image;

    srand(time(NULL));
    cv::VideoCapture cap(0);
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
            for(int i = 0; i < (int)clicked.size(); i++)
                cv::circle(frame, clicked[i], 10, cv::Scalar(0, 0, 255), -1);
            cv::rectangle(frame, cv::Point2i(0, 0), cv::Point2i(CALIB_SIZE, CALIB_SIZE), cv::Scalar(0, 0, 255), -1);
            cv::rectangle(frame, cv::Point2i(0, frame.rows - CALIB_SIZE), cv::Point2i(CALIB_SIZE, frame.rows), cv::Scalar(0, 0, 255), -1);
            cv::rectangle(frame, cv::Point2i(frame.cols - CALIB_SIZE, 0), cv::Point2i(frame.cols, CALIB_SIZE), cv::Scalar(0, 0, 255), -1);
            cv::rectangle(frame, cv::Point2i(frame.cols - CALIB_SIZE, frame.rows - CALIB_SIZE), frame.size(), cv::Scalar(0, 0, 255), -1);
            cv::imshow(WINDOW_NAME, frame);
        } else if(mode == 1) {
            cap >> frame;

            cv::warpPerspective(frame, image, transform, frame.size());
            cv::imshow(WINDOW_NAME, image);
        } else if(mode == 2) {
            cap >> frame;
            frame = cv::Scalar(255, 255, 255);

            if(center == -1) {
                center = rand() % (frame.cols - BOX_DIFF * 2) + BOX_DIFF;
                if(center <= BOX_DIFF) {
                    box = center + BOX_DIFF + rand() % (frame.cols - (center + BOX_DIFF));
                } else if(frame.cols - center <= BOX_DIFF) {
                    box = center - BOX_DIFF - rand() % (center - BOX_DIFF);
                } else {
                    if(rand() % 2)
                        box = center + BOX_DIFF + rand() % (frame.cols - (center + BOX_DIFF));
                    else
                        box = center - BOX_DIFF - rand() % (center - BOX_DIFF);
                }
            }

            cv::rectangle(frame, cv::Point2i(0, 0), cv::Point2i(EDGE_FIELD, frame.rows), cv::Scalar(0, 0, 0), -1);
            cv::rectangle(frame, cv::Point2i(0, 0), cv::Point2i(frame.cols, EDGE_FIELD), cv::Scalar(0, 0, 0), -1);
            cv::rectangle(frame, cv::Point2i(frame.cols - EDGE_FIELD, 0), cv::Point2i(frame.cols, frame.rows), cv::Scalar(0, 0, 0), -1);
            cv::rectangle(frame, cv::Point2i(0, frame.rows - EDGE_FIELD), cv::Point2i(frame.cols, frame.rows), cv::Scalar(0, 0, 0), -1);

            cv::Point2i triangle[3] = {cv::Point2i(center, EDGE_FIELD * 0.75), cv::Point2i(center - EDGE_FIELD * 0.25, EDGE_FIELD * 0.25), cv::Point2i(center + EDGE_FIELD * 0.25, EDGE_FIELD * 0.25)};
            cv::fillConvexPoly(frame, triangle, 3, cv::Scalar(0, 255, 0));
            cv::rectangle(frame, cv::Point2i(box - BOX_SIZE / 2, frame.rows - EDGE_FIELD * 0.75), cv::Point2i(box + BOX_SIZE / 2, frame.rows - EDGE_FIELD * 0.25), cv::Scalar(0, 255, 0), -1);
            
            cv::imshow(WINDOW_NAME, frame);
        } else if(mode == 3) {
            if(timer == 0) {
                cap >> frame;
                cv::warpPerspective(frame, image, transform, frame.size());

                bool color_use[COLOR_NUM] = {false}, used[image.rows][image.cols];
                std::vector<std::pair<cv::Point2i, int>> center;

                memset(used, 0, sizeof(used));
                imos = (double **)malloc(sizeof(double *) * image.cols);
                for(int i = 0; i < image.cols; i++)
                    imos[i] = (double *)malloc(sizeof(double) * image.rows);
                sands.clear();

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
                                center.push_back(std::pair<cv::Point2i, int>(cv::Point2i(moment.m10 / moment.m00, moment.m01 / moment.m00), count));
                                break;
                            }
                        }

                        if(!used[i][j] || i < EDGE || i + EDGE > image.rows || j < EDGE || j + EDGE > image.cols)
                            update_color(image, cv::Point2i(i, j), BACKGROUND);
                    }
                }

                for(int i = 0; i < image.rows; i++) {
                    for(int j = 0; j < image.cols; j++) {
                        imos[j][i] = 0;
                        for(int k = 0; k < (int)center.size(); k++) {
                            if(hypot(center[k].first.x - j, center[k].first.y - k) < 1000) {
                                if(j > center[k].first.x)
                                    imos[j][i] += center[k].second * pow(RATIO, hypot(center[k].first.x - j, center[k].first.y - i));
                                else if(j < center[k].first.x)
                                    imos[j][i] -= center[k].second * pow(RATIO, hypot(center[k].first.x - j, center[k].first.y - i));
                            }
                        }
                    }
                }

                frame = image.clone();

#ifdef DEBUG
                for(int i = 0; i < (int)center.size(); i++)
                    cv::circle(image, center[i].first, 10, cv::Scalar(0, 128, 128, 0), -1);
                cv::imshow(WINDOW_NAME, image);
                cv::waitKey(0);

                double minimum = DBL_MAX, maximum = DBL_MIN;
                for(int i = 0; i < image.rows; i++) {
                    for(int j = 0; j < image.cols; j++) {
                        minimum = std::min(minimum, imos[j][i]);
                        maximum = std::max(maximum, imos[j][i]);
                    }
                }

                for(int i = 0; i < image.rows; i++) {
                    for(int j = 0; j < image.cols; j++) {
                        if(imos[j][i] > 0)
                            update_color(image, cv::Point2i(i, j), cv::Scalar(255 * imos[j][i] / maximum, 0, 0));
                        else if(imos[j][i] < 0)
                            update_color(image, cv::Point2i(i, j), cv::Scalar(0, 255 * abs(imos[j][i]) / abs(minimum), 0));
                    }
                }
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

            cv::Mat field = frame.clone();
            frame = image.clone();

            if(timer < LIMIT) {
                if(timer < TIME) {
                    for(int i = 0; i < AMOUNT; i++) {
                        cv::Point2i sand(rand() % 20, rand() % 20 + center - 10);
                        if(compare_color(field, cv::Point2i(sand.y, sand.x), BACKGROUND))
                            sands.push_back(Sand(sand, cv::Vec2d(rand() % ACCEL + 1, 0), cv::Vec2d(0, 0)));
                    }
                }

                for(std::vector<Sand>::iterator it = sands.begin(); it != sands.end(); it++) {
                    cv::Point2i position = it->getPosition();
                    it->addAcceleration(cv::Vec2d(0, imos[position.y][position.x] / 100000));

                    if(!it->move(frame)) {
                        it = sands.erase(it);
                        it--;
                    }
                }
            } else if(score == -1) {
                score = 0;
                for(int i = 0; i < field.rows; i++)
                    for(int j = 0; j < field.cols; j++)
                        if(compare_color(field, cv::Point2i(i, j), SAND))
                            score++;

                for(int i = 0; i < image.cols; i++)
                    free(imos[i]);
                free(imos);

                std::stringstream ss;
                ss << "Score: " << score / color;
                cv::putText(field, ss.str(), cv::Point2i(50, 50), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 0), 2);

                image = field.clone();
                frame = field;
            }

            cv::imshow(WINDOW_NAME, frame);
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
