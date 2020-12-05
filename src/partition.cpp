#include "geometry.hpp"
#include "parameters.hpp"
#include <iostream>
#include <opencv2/imgproc.hpp>

using std::optional;
using std::pair;
using std::tie;
using std::vector;
using namespace cv;

vector<ImageLine> getCartesianLines(const vector<Vec2f> &polar_lines)
{
    vector<ImageLine> cartesian_lines;

    for (size_t i = 0; i < polar_lines.size(); i++)
    {
        cartesian_lines.push_back(ImageLine(polar_lines[i][0], polar_lines[i][1]));
    }
    return (cartesian_lines);
}

/*
 ** Takes a sorted list of lines and removes lines too close to each other
 ** 1024 = 32 * 32, we remove lines that are less than ~half a square apart
 ** TODO: normalize theta for vertical lines
 */

void removeDuplicateLines(vector<ImageLine> &lines)
{
    float av_theta = 0;

    for (size_t i = 0; i < lines.size(); i++)
    {
        if (M_PI - lines[i].theta > 0.05 && lines[i].theta > 0.05 &&
            abs(M_PI / 2 - lines[i].theta) > 0.05)
        {
            lines.erase(lines.begin() + i);
            i--;
            continue;
        }

        av_theta += lines[i].theta;
    }
    av_theta /= lines.size();
    for (size_t i = 1; i < lines.size(); i++)
    {
        if (euclideanDistSquared(lines[i - 1].center(), lines[i].center()) < 2024)
        {
            if (abs(lines[i - 1].theta - av_theta) < abs(lines[i].theta - av_theta))
            {
                lines.erase(lines.begin() + i);
                i--;
            }
            else
            {
                lines.erase(lines.begin() + i - 1);
                i--;
            }
        }
    }
}

pair<vector<ImageLine>, vector<ImageLine>> getBoardLines(const int threshold, const Mat &edges)
{

    vector<Vec2f> p_hori_lines, p_vert_lines, p_vert_lines2;

    HoughLines(edges, p_hori_lines, 1, CV_PI / 180, threshold, 0, 0, CV_PI / 2 - 0.1,
               CV_PI / 2 + 0.1);
    HoughLines(edges, p_vert_lines, 1, CV_PI / 180, threshold, 0, 0, 0, 0.1);
    HoughLines(edges, p_vert_lines2, 1, CV_PI / 180, threshold, 0, 0, CV_PI - 0.1, CV_PI);

    p_vert_lines.insert(p_vert_lines.end(), p_vert_lines2.begin(), p_vert_lines2.end());

    vector<ImageLine> c_hori_lines, c_vert_lines;
    c_hori_lines = getCartesianLines(p_hori_lines);
    c_vert_lines = getCartesianLines(p_vert_lines);

    sort(c_hori_lines.begin(), c_hori_lines.end(), LineCompareByY());
    sort(c_vert_lines.begin(), c_vert_lines.end(), LineCompareByX());
    removeDuplicateLines(c_hori_lines);
    removeDuplicateLines(c_vert_lines);
    return pair(c_hori_lines, c_vert_lines);
}

void addEdgeLines(vector<ImageLine> &hori_lines, vector<ImageLine> &vert_lines)
{
    hori_lines.push_back(ImageLine(0, 1.57));
    vert_lines.push_back(ImageLine(0, 0));
    hori_lines.push_back(ImageLine(IMG_SIZE, 1.57));
    vert_lines.push_back(ImageLine(IMG_SIZE, 0));
    sort(hori_lines.begin(), hori_lines.end(), LineCompareByY());
    sort(vert_lines.begin(), vert_lines.end(), LineCompareByX());
    removeDuplicateLines(hori_lines);
    removeDuplicateLines(vert_lines);
}

#include "partition.hpp"

vector<vector<Point>> getIntersections(const vector<ImageLine> &hori_lines,
                                       const vector<ImageLine> &vert_lines);

pair<vector<ImageLine>, vector<ImageLine>> findBoardLines(const Mat &edges)
{
    int threshold = 200;
    vector<ImageLine> hori_lines, vert_lines;
    do
    {
        tie(hori_lines, vert_lines) = getBoardLines(threshold, edges);
        if (hori_lines.size() == 7 && vert_lines.size() == 7)
            addEdgeLines(hori_lines, vert_lines);
        threshold--;
        if (threshold < 0)
        {
            dprintf(2, "Could not detect a chess board. (no lines)\n");
            exit(1);
        }
    } while (hori_lines.size() != 9 || vert_lines.size() != 9);
    return pair(hori_lines, vert_lines);
}

// returns points of intersections between horizontal and vertical lines
// in a 9x9 2d array.
// 0,0 ------>
//  |        x
//  | y
//  v
//  points are clamped to image dimensions
//

vector<vector<Point>> getIntersections(const vector<ImageLine> &hori_lines,
                                       const vector<ImageLine> &vert_lines)
{
    vector<vector<Point>> intersections;
    optional<Point> inter;

    intersections.reserve(9);
    for (size_t x = 0; x < vert_lines.size(); x++)
    {
        vector<Point> line;
        line.reserve(9);
        intersections.push_back(line);
        for (size_t y = 0; y < hori_lines.size(); y++)
        {
            inter = intersection(hori_lines[x], vert_lines[y]);
            if (!inter)
            {
                dprintf(2, "Could not detect a chess board. (no intersections)\n");
                exit(1);
            }
            inter->x = std::clamp(inter->x, 0, IMG_SIZE);
            inter->y = std::clamp(inter->y, 0, IMG_SIZE);
            intersections[x].push_back(*inter);
        }
    }
    return (intersections);
}
