#include "StdAfx.h"
#include "Matching.h"


Matching::Matching(Rect roiL, Rect roiR){
	roiLeft=roiL;
	roiRight=roiR;
}

Matching::Matching(void)
{
}


Matching::~Matching(void)
{
}


vector<vector<Point2f>> Matching::siftMatcher(Mat *imageLeftUndistort, Mat *imageRightUndistort, int method){
	//Note: rotate BEFORE computing homographies!
	//Find points of interest via SIFT
	vector<KeyPoint> pointsleft;
	SIFT sift(0,3,0.08,4.0,1.6);
	Mat mask = Mat::zeros(imageLeftUndistort->size(), CV_8U);
	//todo
	mask(roiLeft).setTo(Scalar::all(255.0));
	Mat descriptors1;
	sift.operator()(*imageLeftUndistort,mask,pointsleft,descriptors1,false);

	vector<KeyPoint> pointsright;
	Mat mask2 = Mat::zeros(imageLeftUndistort->size(), CV_8U);
	mask2(roiRight).setTo(Scalar::all(255.0));
	Mat descriptors2;
	sift.operator()(*imageRightUndistort,mask2,pointsright,descriptors2,false);

	//matching
	FlannBasedMatcher flann;
	vector<DMatch> matches1;
	vector<DMatch> matches2;
	flann.match(descriptors1,descriptors2,matches1);
	flann.match(descriptors2,descriptors1,matches2);

	//simple symmetry and vertical disparity test
	vector<DMatch> acceptable_matches;
	for(int i=0;i<matches1.size();i++){
		int first = matches1[i].queryIdx;
		int second = matches1[i].trainIdx;
		for(int j=0;j<matches2.size();j++){
			if(matches2[j].queryIdx==second && matches2[j].trainIdx==first){
				if(std::abs(pointsleft[first].pt.y-pointsright[second].pt.y)<30){
					acceptable_matches.push_back(matches1[i]);
				}
			}
		}
	}
	namedWindow("current match",CV_WINDOW_NORMAL);
	resizeWindow("current match",1200,800);

	//manual selection
	vector<DMatch> goodmatches, badmatches;
	if(method==1){
		for(int i=0;i<acceptable_matches.size();i++){
			DMatch current = acceptable_matches[i];
			bool nearby=false;
			for(int j=0;j<goodmatches.size();j++){
				DMatch currentgood = goodmatches[j];
				if(pointdistance(pointsleft[current.queryIdx],pointsleft[currentgood.queryIdx])<50 && vectordistance(pointsleft[current.queryIdx],pointsleft[currentgood.queryIdx],pointsright[current.trainIdx],pointsright[currentgood.trainIdx])<10){
					nearby=true;
					break;
				}
			}

			for(int j=0;j<badmatches.size();j++){
				DMatch currentbad = badmatches[j];
				if(pointdistance(pointsleft[current.queryIdx],pointsleft[currentbad.queryIdx])<50 && vectordistance(pointsleft[current.queryIdx],pointsleft[currentbad.queryIdx],pointsright[current.trainIdx],pointsright[currentbad.trainIdx])<5){
					nearby=true;
					break;
				}
			}
			if(nearby){
				badmatches.push_back(current);
			}
			else{
				Mat currentmatchimage;
				vector<DMatch> currentMatch;
				currentMatch.push_back(current);
				drawMatches(*imageLeftUndistort,pointsleft,*imageRightUndistort,pointsright,currentMatch,currentmatchimage);
				imshow("current match",currentmatchimage);
				if(waitKey(0)==2555904){
					goodmatches.push_back(current);
				}
				else{
					badmatches.push_back(current);
				}
			}
		}
	}
	else if(method==2){
		goodmatches=acceptable_matches;
	}

	vector<Point2f> pointsL, pointsR;
	vector<vector<Point2f>> result;
	for(int i=0;i<goodmatches.size();i++){
		pointsL.push_back(pointsleft[goodmatches.at(i).queryIdx].pt);
		pointsR.push_back(pointsright[goodmatches.at(i).trainIdx].pt);
	}
	result.push_back(pointsL);
	result.push_back(pointsR);
	destroyWindow("current match");

	return result;
}

double Matching::pointdistance(KeyPoint p, KeyPoint q){
	return std::sqrt((p.pt.x-q.pt.x)*(p.pt.x-q.pt.x)+(p.pt.y-q.pt.y)*(p.pt.y-q.pt.y));
}

double Matching::vectordistance(KeyPoint p1, KeyPoint p2, KeyPoint q1, KeyPoint q2){
	double x = p1.pt.x-p2.pt.x;
	double y = p1.pt.y-p2.pt.y;
	double x2 = q1.pt.x-q2.pt.x;
	double y2 = q1.pt.y-q2.pt.y;
	return std::sqrt((x-x2)*(x-x2)+(y-y2)*(y-y2));
} 

vector<vector<Point2f>> Matching::computeChessboardMatches(Mat *imageLeftUndistort, Mat *imageRightUndistort){
	Size patternsize(8,6);
	vector<Point2f> cornersLeft, cornersRight;

	bool patternfoundLeft = findChessboardCorners(*imageLeftUndistort, patternsize, cornersLeft,
		CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE
		+ CALIB_CB_FAST_CHECK);
	bool patternfoundRight = findChessboardCorners(*imageRightUndistort, patternsize, cornersRight,
		CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE
		+ CALIB_CB_FAST_CHECK);

	vector<vector<Point2f>> result;
	if(patternfoundLeft && patternfoundRight){
		cornerSubPix(*imageLeftUndistort, cornersLeft, Size(11, 11), Size(-1, -1),
		TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
		cornerSubPix(*imageRightUndistort, cornersRight, Size(11, 11), Size(-1, -1),
			TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
		result.push_back(cornersLeft);
		result.push_back(cornersRight);

		vector<DMatch> chessmatches;
		vector<KeyPoint> keypointsleft, keypointsright;
		for(int i=0;i<cornersLeft.size();i++){
			KeyPoint keyleft(cornersLeft[i],1);
			keypointsleft.push_back(keyleft);
			KeyPoint keyright(cornersRight[i],1);
			keypointsright.push_back(keyright);
			DMatch cornermatch(i,i,0);
			chessmatches.push_back(cornermatch);
		}
		Mat currentchessimage;
		drawMatches(*imageLeftUndistort,keypointsleft,*imageRightUndistort,keypointsright,chessmatches,currentchessimage);
		namedWindow("chessboardmatches",CV_WINDOW_NORMAL);
		resizeWindow("chessboardmatches",1200,800);
		imshow("chessboardmatches", currentchessimage);
		waitKey(0);
		destroyWindow("chessboardmatches");
	}
	else{
		//result=computeMatches();
	}
	return result;
}