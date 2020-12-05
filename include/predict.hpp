#pragma once

#include <opencv2/core/types.hpp>
#include <queue>

struct knn_piece
{
    char name;
    float distance;
    knn_piece(char c, float d) : name(c), distance(d)
    {
    }
};

inline bool operator<(const knn_piece &a, const knn_piece &b)
{
    return (a.distance < b.distance);
}

std::priority_queue<knn_piece> find_knn(const cv::Mat &piece, const size_t k);
char classifyPiece(const cv::Mat &piece);
