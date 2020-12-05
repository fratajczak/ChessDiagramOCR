#pragma once
#include <opencv2/core/types.hpp>
#include <optional>

using std::vector;

std::optional<cv::Mat> extractPotentialPiece(const cv::Mat &gray_board,
                                             const vector<vector<cv::Point>> &intersections, int x,
                                             int y);
bool isOnBlackSquare(cv::Mat &piece);
