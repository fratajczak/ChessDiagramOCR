#pragma once

#include <opencv2/core/types.hpp>
#include <utility>

using cv::Point;
using std::optional;

class ImageLine
{
  public:
    float theta;
	float r;
    ImageLine(const float r, const float theta);
    ImageLine(const Point p1, const Point p2);
    Point center() const;
    std::pair<Point, Point> coords() const;

  private:
    Point p1;
    Point p2;
    Point p_center;
};

struct LineCompareByX
{
    bool operator()(const ImageLine &line1, const ImageLine &line2)
    {
        return (line1.center().x < line2.center().x);
    }
};

struct LineCompareByY
{
    bool operator()(const ImageLine &line1, const ImageLine &line2)
    {
        return (line1.center().y < line2.center().y);
    }
};

float euclideanDistSquared(const Point &p1, const Point &p2);
optional<Point> intersection(const ImageLine &l1, const ImageLine &l2);
