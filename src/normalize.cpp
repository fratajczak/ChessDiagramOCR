#include "parameters.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using std::vector;

#ifdef DEBUG
void drawBoundingBoxes(const vector<vector<Point>> &contours, Rect biggest_box, const Mat &piece)
{
    Rect box;
    Mat img;

    cvtColor(piece, img, COLOR_GRAY2RGB);
    for (size_t i = 0; i < contours.size(); i++)
    {
        box = boundingRect(contours[i]);
        if (box.x < 2 || box.y < 2 || box.x + box.width > piece.size[0] - 2 ||
            box.y + box.height > piece.size[1] - 2)
        {
            if (box.width < 10 || box.height < 10)
            {
                drawContours(img, contours, i, Scalar(255, 0, 255));
                rectangle(img, box, Scalar(0, 0, 255));
                continue;
            }
        }
        rectangle(img, box, Scalar(0, 255, 0));
        drawContours(img, contours, i, Scalar(0, 255, 255));
    }
    rectangle(img, biggest_box, Scalar(255, 0, 0));

    imshow("boundingBox", img);
    waitKey();
}
#endif

Rect getBoundingBox(const Mat &piece)
{
    Mat piece_contour;
    vector<vector<Point>> contours;

    blur(piece, piece_contour, {3, 3});
    threshold(piece_contour, piece_contour, 0, 255, THRESH_OTSU);
    findContours(piece_contour, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

    for (size_t i = 0; i < contours.size(); i++)
    {
        if (boundingRect(contours[i]).area() > 0.85 * piece.size[0] * piece.size[1])
        {
            contours.erase(contours.begin() + i);
            i--;
        }
    }

    Point best_box[2]; // first point is top left, second is bottom right
    Rect box;

    best_box[0] = Point(IMG_SIZE, IMG_SIZE);
    best_box[1] = Point(0, 0);
    for (auto &contour : contours)
    {
        box = boundingRect(contour);
        // skip parts of adjacent squares on the edge
        if (box.x < 2 || box.y < 2 || box.x + box.width > piece.size[0] - 2 ||
            box.y + box.height > piece.size[1] - 2)
        {
            if (box.width < 10 || box.height < 10)
                continue;
        }
        if (box.x < best_box[0].x)
            best_box[0].x = box.x;
        if (box.y < best_box[0].y)
            best_box[0].y = box.y;
        if (box.x + box.width > best_box[1].x)
            best_box[1].x = box.x + box.width;
        if (box.y + box.height > best_box[1].y)
            best_box[1].y = box.y + box.height;
    }
    box.x = best_box[0].x;
    box.y = best_box[0].y;
    box.width = best_box[1].x - best_box[0].x;
    box.height = best_box[1].y - best_box[0].y;
#ifdef DEBUG
    drawBoundingBoxes(contours, box, piece);
#endif
    return (box);
}

// Takes a normalized piece as argument

bool isOnBlackSquare(Mat &piece)
{
    int black_corners;

    black_corners =
        (mean(piece(Rect(0, 0, 4, 4)))[0] < 10) + (mean(piece(Rect(28, 0, 4, 4)))[0] < 10) +
        (mean(piece(Rect(0, 28, 4, 4)))[0] < 10) + (mean(piece(Rect(28, 28, 4, 4)))[0] < 10);

    return (black_corners >= 2);
}

void normalizePiece(Mat &piece)
{
    int border_v = 0;
    int border_h = 0;

    if (piece.size[0] < piece.size[1])
        border_v = (piece.size[1] - piece.size[0]) / 2;
    else
        border_h = (piece.size[0] - piece.size[1]) / 2;
    copyMakeBorder(piece, piece, border_v, border_v, border_h, border_h, BORDER_CONSTANT,
                   Scalar(255, 255, 255));
    resize(piece, piece, Size(32, 32), INTER_AREA);
}

void removeDiagonalLines(Mat &img)
{
    static const Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(1, 5));
    static const Mat verticalStructure = getStructuringElement(MORPH_RECT, Size(5, 1));
    static const Mat diagonalStructure = Mat::eye(Size(5, 5), CV_8U);

    Mat hori;
    Mat vert;
    Mat diag;

    bitwise_not(img, hori);
    vert = hori.clone();
    diag = hori.clone();

    erode(hori, hori, horizontalStructure);
    dilate(hori, hori, horizontalStructure);
    erode(vert, vert, verticalStructure);
    dilate(vert, vert, verticalStructure);
    erode(diag, diag, diagonalStructure);
    dilate(diag, diag, diagonalStructure);

    add(vert, hori, img);
    add(diag, img, img);
    bitwise_not(img, img);
}

std::optional<Mat> extractPotentialPiece(const Mat &gray_board,
                                         const vector<vector<Point>> &intersections, int x, int y)
{
    std::optional<Mat> piece;
    Mat square;
    Point p = intersections[x][y];

    Rect square_location =
        Rect(p.x, p.y, intersections[x][y + 1].x - p.x, intersections[x + 1][y].y - p.y);
    square = gray_board(square_location).clone();

    Scalar mean, stddev;
    meanStdDev(square, mean, stddev);
    // filters empty squares that would give a messy threshold
    if (stddev[0] > 20)
    {
        threshold(square, square, 0, 255, THRESH_OTSU);
        removeDiagonalLines(square);
        Rect box;
        box = getBoundingBox(square);
        if (box.width > 20 && box.height > 20)
        {
            piece = square(box);
            normalizePiece(*piece);
        }
    }
    return (piece);
}
