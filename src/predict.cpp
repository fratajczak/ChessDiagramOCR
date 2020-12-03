#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "predict.hpp"
#include <map>

using namespace cv;
using std::vector;
using std::priority_queue;

constexpr char piece_names[] = "bknpqrBKNPQR";

void load_images(vector<vector<Mat>> & images)
{
	String folder = "pieces/";

	images.reserve(strlen(piece_names));
	for (size_t i = 0; i < strlen(piece_names); i++)
	{
		vector<String> files;
		vector<Mat> piece_imgs;

		glob(folder + piece_names[i], files, false);
		for (auto & file : files)
		{
			piece_imgs.push_back(imread(file, IMREAD_GRAYSCALE));
		}
		images.push_back(piece_imgs);
	}
}

priority_queue<knn_piece> find_knn(const Mat & piece, const size_t k)
{
	priority_queue<knn_piece> neighbors;
	static vector<vector<Mat>> images;
	
	if (images.size() == 0)
		load_images(images);
	for (size_t i = 0; i < strlen(piece_names); i++)
	{
		for (auto & img : images[i])
		{
			float dist = norm(piece, img);
			if (neighbors.size() < k)
				neighbors.push(knn_piece(piece_names[i], dist));
			else if (neighbors.top().distance > dist)
			{
				neighbors.pop();
				neighbors.push(knn_piece(piece_names[i], dist));
			}
		}
	}
	return (neighbors);
}

void removeFurthestPiece(std::multimap<char, float> & map)
{
	std::pair<char, float> furthestPiece = {'a', -1};

	for (auto & p : map)
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

char classifyPiece(const Mat & piece)
{
	priority_queue<knn_piece> neighbors;

	neighbors = find_knn(piece, 20);

	std::multimap<char, float> map;

	while (neighbors.size() != 0)
	{
#ifdef DEBUG
		printf("%c, %f\n", neighbors.top().name, neighbors.top().distance);
#endif
		map.insert({neighbors.top().name, neighbors.top().distance});
		neighbors.pop();
	}
	
	std::pair<char, size_t> majority = {'a', 0};
	std::pair<char, size_t> majority2 = {'a', 0};

	while (majority.second == majority2.second)
	{
		for (auto & c : piece_names)
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
	return (majority.first);
}
