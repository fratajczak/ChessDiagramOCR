#include "../include/geometry.hpp"
#include <utility>

ImageLine::ImageLine(const float r, const float theta)
{
    double cos_t = cos(theta), sin_t = sin(theta);
    double x0 = r * cos_t, y0 = r * sin_t;
    double alpha = 1000;

    p1 = Point(cvRound(x0 + alpha * (-sin_t)), cvRound(y0 + alpha * cos_t));
    p2 = Point(cvRound(x0 - alpha * (-sin_t)), cvRound(y0 - alpha * cos_t));
    p_center = Point((p1.x + p2.x) / 2, (p1.y + p2.y) / 2);
    this->theta = theta;
	this->r = r;
}

ImageLine::ImageLine(const Point p1, const Point p2)
{
    this->p1 = p1;
    this->p2 = p2;
}

Point ImageLine::center() const
{
    return p_center;
}

std::pair<Point, Point> ImageLine::coords() const
{
    return (std::pair(p1, p2));
}

float euclideanDistSquared(const Point &p1, const Point &p2)
{
    return (pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

std::optional<Point> intersection(const ImageLine &l1, const ImageLine &l2)
{
    std::optional<Point> inter;
    Point l1_a, l1_b, l2_a, l2_b;

    std::tie(l1_a, l1_b) = l1.coords();
    std::tie(l2_a, l2_b) = l2.coords();

    Point x = l2_a - l1_a;
    Point d1 = l1_b - l1_a;
    Point d2 = l2_b - l2_a;

    float cross = d1.x * d2.y - d1.y * d2.x;
    if (abs(cross) < 1e-8)
        return (inter);

    double t1 = (x.x * d2.y - x.y * d2.x) / cross;
    inter = l1_a + d1 * t1;
    // printf("%d, %d\n", (*inter).x, (*inter).y);
    return (inter);
}
