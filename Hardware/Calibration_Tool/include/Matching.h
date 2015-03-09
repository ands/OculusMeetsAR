#pragma once

using namespace cv;
//class for keypoint extraction and matching
class Matching
{
public:
	//roiL, roiR specify the image sections for the keypoint extraction 
	Matching(Rect roiL, Rect roiR);
	Matching(void);
	~Matching(void);

	//Keypoint extraction via SIFT (and optional additional search for chessboard corners). Basic matching via openCV's flann-matcher. Optional manual selection of matches.
	//Method: 1-SIFT+Manual selection 2-SIFT+Automatching 4-SIFT+Chessboard+Manual matching 5-SIFT+Chessboard+Automatching
	vector<vector<Point2f>> Matching::siftMatcher(Mat *imageLeftUndistort, Mat *imageRightUndistort, int method, int patternX=0, int patternY=0);

	//Uses the openCV function findchessboardcorners to identify chessboards of size patternX*patternY in the two given images.
	//Returns the corresponding chessboard corners
	vector<vector<Point2f>> Matching::computeChessboardMatches(Mat *imageLeftUndistort, Mat *imageRightUndistort,int patternX, int patternY);

private:
	double Matching::pointdistance(Point2f p, Point2f q);
	Rect roiLeft;
	Rect roiRight;
};

