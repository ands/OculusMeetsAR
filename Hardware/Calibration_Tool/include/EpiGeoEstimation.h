#pragma once
#include "StdAfx.h"

using namespace cv;
class EpiGeoEstimation
{
public:
	EpiGeoEstimation(void);
	~EpiGeoEstimation(void);

	void EpiGeoEstimation::fundamentalHartley(vector<Point2f> leftmatch, vector<Point2f> rightmatch);
	void EpiGeoEstimation::fundamentalZMEK(vector<Point2f> leftmatch, vector<Point2f> rightmatch);
	void EpiGeoEstimation::homographyError(vector<Point2f> left, vector<Point2f> right);
	void EpiGeoEstimation::homographyEstimation(vector<Point2f> leftmatch,vector<Point2f> rightmatch);
	bool EpiGeoEstimation::loadMatrices(const char *fundFile, const char *leftFile, const char *rightFile);
	bool EpiGeoEstimation::saveMatrices(const char *fundFile, const char *leftFile, const char *rightFile);
	

	Mat F;
	Mat leftHomography;
	Mat rightHomography;

private:
	double EpiGeoEstimation::transformDistance(Mat F, Point2f left, Point2f right);
	Mat EpiGeoEstimation::linearSolution(vector<Point2f> left, vector<Point2f> right);
	bool EpiGeoEstimation::saveMatrix(const char *fileName, Mat A);
	bool EpiGeoEstimation::loadMatrix(const char *file, Mat *A);
	bool calculatedByZMEK;
};

