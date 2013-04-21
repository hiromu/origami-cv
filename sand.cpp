#include "sand.h"

Sand::Sand(cv::Point2i coord, cv::Vec2d accel, cv::Vec2d speed)
{
    coordinate = coord;
    acceleration = accel;
    velocity = speed;
}

void Sand::addAcceleration(cv::Vec2d accel)
{
    acceleration += -accel;
}

cv::Point2i Sand::getPosition(void)
{
    return coordinate;
}

bool Sand::move(cv::Mat &image)
{
    if(rand() % 100 < SPEED) {
        velocity += acceleration;
        velocity[1] *= 0.85;

        int i, x = velocity[0], y = velocity[1] + rand() % (LEFT + RIGHT) - LEFT;
        for(i = 0; i < MOVE_CHECK; i++)
            if(!compare_color(image, cv::Point2i(coordinate.x + x * i / MOVE_CHECK, coordinate.y + y * i / MOVE_CHECK), BACKGROUND))
                break;

        coordinate.x += x * i / MOVE_CHECK;
        coordinate.y += y * i / MOVE_CHECK;

        if(coordinate.x < 0 || image.rows <= coordinate.x)
            return false;
        if(coordinate.y < 0 || image.cols <= coordinate.y)
            return false;

        update_color(image, coordinate, SAND);
    }
    return true;
}
