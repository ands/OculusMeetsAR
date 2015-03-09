#pragma once
#include "StdAfx.h"

using namespace cv;
//class for estimation of epipolar geometry
class EpiGeoEstimation
{
public:
	EpiGeoEstimation(void);
	~EpiGeoEstimation(void);

	//calculates the fundamental matrix using the openCV implementation of Hartley's 8-point-algorithm/RANSAC
	void EpiGeoEstimation::fundamentalHartley(vector<Point2f> leftmatch, vector<Point2f> rightmatch);
	//calculates the fundamental matrix using the a method provided by Zilly, Müller, Eisert, Kauff
	//"Joint Estimation of Epipolar Geometry and Rectification Parameters using Point Correspondences for Stereoscopic TV Sequences", 2011
	void EpiGeoEstimation::fundamentalZMEK(vector<Point2f> leftmatch, vector<Point2f> rightmatch);
	//calculates homography accuracy based on the matchingerror
	void EpiGeoEstimation::homographyError(vector<Point2f> left, vector<Point2f> right);
	//Homography estimation. If F is calculated using the ZMEK-function, there is an option to estimate the homographies according to the same paper
	void EpiGeoEstimation::homographyEstimation(vector<Point2f> leftmatch,vector<Point2f> rightmatch);

	bool EpiGeoEstimation::loadMatrices(const char *fundFile, const char *leftFile, const char *rightFile);
	bool EpiGeoEstimation::saveMatrices(const char *fundFile, const char *leftFile, const char *rightFile);
	

	Mat F;
	Mat leftHomography;
	Mat rightHomography;

private:
	//calculates sampson error
	double EpiGeoEstimation::transformDistance(Mat F, Point2f left, Point2f right);
	//fundamental matrix estimation according to ZMEK11
	Mat EpiGeoEstimation::linearSolution(vector<Point2f> left, vector<Point2f> right);

	bool EpiGeoEstimation::saveMatrix(const char *fileName, Mat A);
	bool EpiGeoEstimation::loadMatrix(const char *file, Mat *A);
	bool calculatedByZMEK;
};

