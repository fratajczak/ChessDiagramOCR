#include "predict.hpp"
#include <map>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using std::vector;

constexpr char piece_names[] = "bknpqrBKNPQR";
vector<vector<Mat>> images;

void load_images(vector<vector<Mat>> &images)
{
    String folder = "pieces/";

    images.reserve(strlen(piece_names));
    for (size_t i = 0; i < strlen(piece_names); i++)
    {
        vector<String> files;
        vector<Mat> piece_imgs;

        glob(folder + piece_names[i], files, false);
        for (auto &file : files)
        {
            piece_imgs.push_back(imread(file, IMREAD_GRAYSCALE));
        }
        images.push_back(piece_imgs);
    }
}

void removeFurthestPiece(std::multimap<char, float> &map)
{
    std::pair<char, float> furthestPiece = {'a', -1};

    for (auto &p : map)
    {
        if (p.second > furthestPiece.second)
            furthestPiece = p;
    }

    auto iterpair = map.equal_range(furthestPiece.first);

    for (auto it = iterpair.first; it != iterpair.second; it++)
    {
        if (it->second == furthestPiece.second)
        {
            map.erase(it);
            break;
        }
    }
}

char getMajority(vector<knn_piece>::const_iterator start, vector<knn_piece>::const_iterator end)
{
    std::multimap<char, float> map;

    for (auto p = start; p != end; p++)
    {
        map.insert({p->name, p->distance});
    }

    std::pair<char, size_t> majority = {'a', 0};
    std::pair<char, size_t> majority2 = {'a', 0};

    while (majority == majority2)
    {
        for (auto &c : piece_names)
        {
            size_t n = map.count(c);
            if (n > majority.second)
            {
                majority2 = majority;
                majority = {c, n};
            }
        }
        removeFurthestPiece(map);
    }
    if (majority == majority2)
        return ('0');
    return (majority.first);
}

int findOptimalK(const vector<knn_piece> &pieces)
{
    size_t k = 0;
    bool isCorrectMajority = false;

    while (!isCorrectMajority && k < 100)
    {
        k++;
        if (getMajority(pieces.begin() + 1, pieces.begin() + 1 + k) == pieces[0].name)
            isCorrectMajority = true;
    }
    return (k);
}

char classifyPiece(const Mat &piece)
{

    if (images.size() == 0)
        load_images(images);

    vector<knn_piece> pieces;

    for (size_t i = 0; i < strlen(piece_names); i++)
    {
        for (auto &img : images[i])
        {
            pieces.push_back(knn_piece(piece_names[i], norm(piece, img)));
        }
    }
    std::sort(pieces.begin(), pieces.end());

    int k = findOptimalK(pieces);
    return (getMajority(pieces.begin(), pieces.begin() + k));
}
