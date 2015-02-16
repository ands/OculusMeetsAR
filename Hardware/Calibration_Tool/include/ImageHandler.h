#pragma once
#include "Undistortion.h"
#include "Matching.h"
#include "EpiGeoEstimation.h"

using namespace cv;
class ImageHandler
{
public:
	ImageHandler(void);
	~ImageHandler(void);
	void ImageHandler::loadImagePairs(vector<const char*> imagepathLeft, vector<const char*> imagepathRight);
	void ImageHandler::undistortAndRotate();
	void ImageHandler::calculateMatches(int method);
	void ImageHandler::estimateGeometry(int method);
	void ImageHandler::visualize();
	void ImageHandler::saveMatches();
	void ImageHandler::loadMatches();
	void ImageHandler::saveEpiGeo(bool normalized);
	void ImageHandler::loadEpiGeo();

	int numOfImages;
	vector<Point2f> leftMatch;
	vector<Point2f> rightMatch;
	vector<Mat> leftImages;
	vector<Mat> rightImages;
	
private:
	double ImageHandler::dist(Point2f p, Point2f q);
	void ImageHandler::saveImg(Mat img, string name);
	float ImageHandler::distancePointLine(const cv::Point2f point, const Vec<float,3>& line);
	void ImageHandler::drawEpipolarLines(const std::string& title, const Mat F,
	const cv::Mat& img1, const cv::Mat& img2,
	const std::vector<cv::Point2f> points1,
	const std::vector<cv::Point2f> points2,
	const float inlierDistance);

	Undistortion leftUndistortion;
	Undistortion rightUndistortion;
	Matching matcher;
	EpiGeoEstimation epigeo;
};

