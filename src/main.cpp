#include "fen.hpp"
#include "geometry.hpp"
#include "normalize.hpp"
#include "parameters.hpp"
#include "partition.hpp"
#include "predict.hpp"
#include <getopt.h>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <queue>
#include <stdlib.h>
#include <time.h>

using namespace cv;
using namespace std;

std::string option_desc[3][2] = {
    {"--correct-fen=FEN",
     "classifies pieces according to a FEN position when using --output-folder"},
    {"--output-folder=FOLDER", "outputs normalized piece images in FOLDER to use for training"},
    {"-h, --help", "display this help and exit"}};

char *output_folder = NULL;
char *correct_fen = NULL;

void print_help_and_exit()
{
    printf("usage: chessOCR [OPTIONS] [IMAGE]\n");
    exit(0);
}

void parse_options(int argc, char **argv)
{
    int c;
    int option_index = 0;

    static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                           {"output-folder", required_argument, 0, 0},
                                           {"correct-fen", required_argument, 0, 0},
                                           {0, 0, 0, 0}};

    while ((c = getopt_long(argc, argv, "h", long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 0:
            if (strcmp(long_options[option_index].name, "output-folder") == 0)
                output_folder = optarg;
            else
                correct_fen = optarg;
            break;
        case 'h':
            print_help_and_exit();
            break;
        }
    }
}

int main(int argc, char **argv)
{
    String imageName;

    parse_options(argc, argv);
    if (optind < argc)
        imageName = argv[optind];
    else
    {
        dprintf(2, "usage: ./chessOCR [OPTIONS] [IMAGE]\n");
        exit(1);
    }

    char correct_board[8][8] = {{0}};

    // fill_board_from_fen(correct_board, correct_fen);

    Mat src, edges;

    src = imread(samples::findFile(imageName), IMREAD_GRAYSCALE);

    resize(src, src, {IMG_SIZE, IMG_SIZE});
    blur(src, edges, {7, 7});
    Canny(edges, edges, 61, 550, 5);

    vector<ImageLine> hori_lines, vert_lines;

    tie(hori_lines, vert_lines) = findBoardLines(edges);

    vector<vector<Point>> intersections = getIntersections(hori_lines, vert_lines);

#ifdef DEBUG
    drawPartition(src, intersections, hori_lines, vert_lines);
#endif

    char predicted_board[8][8] = {{0}};

    for (auto x = 0; x <= 7; x++)
    {
        for (auto y = 0; y <= 7; y++)
        {
            optional<Mat> piece = extractPotentialPiece(src, intersections, x, y);
            if (!piece && correct_board[y][x] != 0)
                printf("WARNING: filtered real piece on %d, %d\n", x, y);
            if (piece) //&& correct_board[y][x])
            {
                bool onBlack = isOnBlackSquare(*piece);
                if (onBlack)
                {
                    bitwise_not(*piece, *piece);
                    reverse_piece_color(correct_board[y][x]);
                }
                char piece_class = classifyPiece(*piece);
                if (onBlack)
                    reverse_piece_color(piece_class);
                predicted_board[y][x] = piece_class;
#ifdef DEBUG
                printf("%c\n", classifyPiece(*piece));
                imshow("pice", *piece);
                waitKey();
#endif
            }
        }
    }
    printf("%s\n", get_fen_from_board(predicted_board).c_str());
#ifdef DEBUG
    waitKey();
#endif
    return 0;
}
