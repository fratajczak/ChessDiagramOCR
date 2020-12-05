#pragma once

#include "geometry.hpp"
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <utility>

using cv::Mat;
using cv::Point;
using cv::Vec2f;
using std::pair;
using std::vector;

pair<vector<ImageLine>, vector<ImageLine>> findBoardLines(const Mat &edges);
vector<vector<Point>> getIntersections(const vector<ImageLine> &hori_lines,
                                       const vector<ImageLine> &vert_lines);
void drawPartition(Mat image, const vector<vector<Point>> &intersections,
                   const vector<ImageLine> &hori_lines, const vector<ImageLine> &vert_lines);
