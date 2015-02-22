#pragma once

using namespace cv;
class Matching
{
public:
	Matching(Rect roiL, Rect roiR);
	Matching(void);
	~Matching(void);
	vector<vector<Point2f>> Matching::siftMatcher(Mat *imageLeftUndistort, Mat *imageRightUndistort, int method);
	vector<vector<Point2f>> Matching::computeChessboardMatches(Mat *imageLeftUndistort, Mat *imageRightUndistort);

private:
	double Matching::pointdistance(KeyPoint p, KeyPoint q);
	double Matching::vectordistance(KeyPoint p1, KeyPoint p2, KeyPoint q1, KeyPoint q2);
	Rect roiLeft;
	Rect roiRight;
};

