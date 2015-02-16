#pragma once
#include "StdAfx.h"

using namespace cv;
class EpiGeoEstimation
{
public:
	EpiGeoEstimation(void);
	~EpiGeoEstimation(void);

	void EpiGeoEstimation::homographyHartley(vector<Point2f> leftmatch, vector<Point2f> rightmatch);
	void EpiGeoEstimation::homographyZMEK(vector<Point2f> leftmatch, vector<Point2f> rightmatch);
	double EpiGeoEstimation::transformDistance(Mat F, Point2f left, Point2f right);
	Mat EpiGeoEstimation::linearSolution(vector<Point2f> left, vector<Point2f> right);

	bool EpiGeoEstimation::loadMatrices(const char *fundFile, const char *leftFile, const char *rightFile);
	bool EpiGeoEstimation::loadMatrix(const char *file, Mat *A);
	bool EpiGeoEstimation::saveMatrices(const char *fundFile, const char *leftFile, const char *rightFile, bool normalized);
	bool EpiGeoEstimation::saveMatrix(const char *fileName, Mat A);

	Mat F;
	Mat leftHomography;
	Mat rightHomography;
};

