#include "geometry.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <utility>

using namespace cv;
using std::tie;
using std::vector;

void drawLines(Mat &image, const vector<ImageLine> &lines)
{
    for (size_t i = 0; i < lines.size(); i++)
    {
        Point pt1, pt2;
        tie(pt1, pt2) = lines[i].coords();
        line(image, pt1, pt2, Scalar(255, 0, 0), 3, LINE_AA);
    }
}

void drawIntersections(Mat &image, const vector<vector<Point>> &intersections)
{
    for (const vector<Point> &line : intersections)
    {
        for (const Point &inter : line)
        {
            circle(image, inter, 10, Scalar(0, 0, 255));
        }
    }
}

void drawPartition(Mat image, const vector<vector<Point>> &intersections,
                   const vector<ImageLine> &hori_lines, const vector<ImageLine> &vert_lines)

{
    drawLines(image, hori_lines);
    drawLines(image, vert_lines);
    drawIntersections(image, intersections);
    imshow("lines", image);
    waitKey();
}
