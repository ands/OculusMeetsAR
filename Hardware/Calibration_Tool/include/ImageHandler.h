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
	//loads images from imagepathLeft and imagepathRight to vectors leftImages and rightImages
	void ImageHandler::loadImagePair(string imagepathLeft, string imagepathRight);
	//applies undistortion according to ocam-models given by the calib_results_CAM*.txt
	void ImageHandler::undistortAndRotate();
	//calculates matches from loaded image pairs
	//method: 1=SIFT+Manual selection, 2=SIFT+Autoselection, 3=Chessboardmatcher
	void ImageHandler::calculateMatches(int method);
	//estimates the epipolar geometry from previously calculated matches
	//method: 1=Hartley's 8point-algorithm+RANSAC, 2=method after Zilly, Müller, Eisert, Kauff
	void ImageHandler::estimateGeometry(int method);
	//Show epipolarlines for first image pair and save one rectified image pair in media folder
	void ImageHandler::visualize();

	//self-explanatory: save/load functions for matches and epipolar geometry/homographies
	void ImageHandler::saveMatches();
	void ImageHandler::loadMatches();
	void ImageHandler::saveEpiGeo();
	void ImageHandler::loadEpiGeo();

	//number of image pairs currently loaded
	int numOfImages;
	//vectors containing the left/right matching points
	vector<Point2f> leftMatch;
	vector<Point2f> rightMatch;
	
private:
	//Undistortion classes manage the ocam-models
	Undistortion leftUndistortion;
	Undistortion rightUndistortion;
	//Matchingclass
	Matching matcher;
	//epigeo manages the estimation of the epipolar geometry
	EpiGeoEstimation epigeo;
	//currently loaded imagepairs
	vector<Mat> leftImages;
	vector<Mat> rightImages;

	//calculates point distance
	double ImageHandler::dist(Point2f p, Point2f q);
	//saves img to file
	void ImageHandler::saveImg(Mat img, string name);
	//calculates distance from point to line
	float ImageHandler::distancePointLine(const cv::Point2f point, const Vec<float,3>& line);
	//draws epipolar lines according to fundamental matrix F onto the image pair img1, img2
	void ImageHandler::drawEpipolarLines(const Mat F,
		const cv::Mat& img1, const cv::Mat& img2,
		const std::vector<cv::Point2f> points1,
		const std::vector<cv::Point2f> points2);
};

