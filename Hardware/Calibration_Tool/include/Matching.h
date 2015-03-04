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
	double Matching::pointdistance(Point2f p, Point2f q);
	Rect roiLeft;
	Rect roiRight;
};

