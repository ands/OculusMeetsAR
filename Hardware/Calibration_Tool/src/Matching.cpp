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


vector<vector<Point2f>> Matching::siftMatcher(Mat *imageLeftUndistort, Mat *imageRightUndistort, int method,int patternX, int patternY){
	//Note: rotate BEFORE computing homographies!
	//Copy both images into one
	Mat bothImagesG(imageLeftUndistort->rows,imageLeftUndistort->cols+imageRightUndistort->cols,imageLeftUndistort->type());
	Rect rectleft= Rect(0, 0, imageLeftUndistort->cols, imageLeftUndistort->rows);
	Mat left(bothImagesG, rectleft);
    imageLeftUndistort->copyTo(left);
    Rect rectright(imageLeftUndistort->cols, 0, imageRightUndistort->cols, imageRightUndistort->rows);
	Mat right(bothImagesG, rectright);
    imageRightUndistort->copyTo(right);
	//gray to color conversion for showing colored lines
	Mat bothImages;
	cvtColor(bothImagesG,bothImages,CV_GRAY2RGB);
	//for imshow purpose
	Scalar color(50,50,255);
	Point2f addPoint = Point2f(imageLeftUndistort->cols,0);

	vector<Point2f> matchLeft, matchRight, matchCandidateLeft, matchCandidateRight, badMatchLeft, badMatchRight;

	//optional chessboard matching
	int numOfChessmatches=0;
	if(method==4||method==5){
		vector<vector<Point2f>> corners = computeChessboardMatches(imageLeftUndistort,imageRightUndistort,patternX,patternY);
		if(corners.size()!=0){
			matchLeft=corners[0];
			matchRight=corners[1];
			numOfChessmatches=matchLeft.size();
		}
	}

	//Find points of interest via SIFT using the ROIs given by ocam-undistortion
	vector<KeyPoint> pointsleft;
	SIFT sift(0,3,0.08,4.0,1.6);
	Mat mask = Mat::zeros(imageLeftUndistort->size(), CV_8U);
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

	//save SIFT-matches as Point2f-vectors
	for(int i=0;i<acceptable_matches.size();i++){
		DMatch current = acceptable_matches[i];
		matchCandidateLeft.push_back(pointsleft[current.queryIdx].pt);
		matchCandidateRight.push_back(pointsright[current.trainIdx].pt);
	}

	int numOfMatches = matchCandidateLeft.size();

	namedWindow("current match",CV_WINDOW_NORMAL);
	resizeWindow("current match",1200,800);

	//manual selection
	if(method==1||method==4){
		for(int i=0;i<numOfMatches;i++){
			Point2f curPointL = matchCandidateLeft[i];
			Point2f curPointR = matchCandidateRight[i];
			bool nearby=false;
			//Test whether there are similar good matches nearby
			for(int j=0;j<numOfChessmatches;j++){//compare to chessboardmatches
				if(pointdistance(matchLeft[j],curPointL)<50){
					nearby=true;
					break;
				}
			}
			for(int j=numOfChessmatches;j<matchLeft.size();j++){//compare to non-chessboardmatches
				if(pointdistance(matchLeft[j],curPointL)<50 && pointdistance(curPointL-matchLeft[j],curPointR-matchRight[j])<10){
					nearby=true;
					break;
				}
			}
			//Test whether there are similar bad matches nearby
			for(int j=0;j<badMatchLeft.size();j++){
				if(pointdistance(badMatchLeft[j],curPointL)<50 && pointdistance(curPointL-badMatchLeft[j],curPointR-badMatchRight[j])<5){
					nearby=true;
					break;
				}
			}
			if(nearby){//if there are bad/good matches near the current match, it is useless (either too bad for estimation of the fundamental matrix or too similar to an already saved match)
				badMatchLeft.push_back(curPointL);
				badMatchRight.push_back(curPointR);
			}
			else{//show current match
				Mat bothImagesCopy = bothImages.clone();

				line(bothImagesCopy,curPointL,curPointR+addPoint,color,2);
				circle(bothImagesCopy,curPointL,10,color,1);
				circle(bothImagesCopy,curPointR+addPoint,10,color,1);
				imshow("current match", bothImagesCopy);

				if(waitKey(0)==2555904){//press right arrow key to accept
					matchLeft.push_back(curPointL);
					matchRight.push_back(curPointR);
				}
				else{//or any other key to reject the match
					badMatchLeft.push_back(curPointL);
					badMatchRight.push_back(curPointR);
				}
			}
		}
	}
	else if(method==2||method==5){//no manual selection
		for(int i=0;i<matchCandidateLeft.size();i++){
			Point2f curPointL = matchCandidateLeft[i];
			Point2f curPointR = matchCandidateRight[i];
			bool nearby=false;
			for(int j=0;j<numOfChessmatches;j++){
				if(pointdistance(matchLeft[j],curPointL)<50){
					nearby=true;
					break;
				}
			}
			for(int j=numOfChessmatches;j<matchLeft.size();j++){
				if(pointdistance(matchLeft[j],curPointL)<50 && pointdistance(curPointL-matchLeft[j],curPointR-matchRight[j])<10){
					nearby=true;
					break;
				}
			}
			if(!nearby){
				matchLeft.push_back(curPointL);
				matchRight.push_back(curPointR);
			}
		}
	}
	destroyWindow("current match");

	//show all matches
	Mat bothImagesCopy = bothImages.clone();
	RNG rng;
	for(int i=0;i<matchLeft.size();i++){
		Point2f curPointL = matchLeft[i];
		Point2f curPointR = matchRight[i];
		color=Scalar(rng(255),rng(255),rng(255));
		line(bothImagesCopy,curPointL,curPointR+addPoint,color,2);
		circle(bothImagesCopy,curPointL,10,color,1);
		circle(bothImagesCopy,curPointR+addPoint,10,color,1);
	}
	namedWindow("All matches",CV_WINDOW_NORMAL);
	resizeWindow("All matches",1200,800);
	imshow("All matches", bothImagesCopy);
	waitKey(0);
	destroyWindow("All matches");

	vector<vector<Point2f>> result;
	result.push_back(matchLeft);
	result.push_back(matchRight);
	return result;
}

double Matching::pointdistance(Point2f p, Point2f q){
	return std::sqrt((p.x-q.x)*(p.x-q.x)+(p.y-q.y)*(p.y-q.y));
}

vector<vector<Point2f>> Matching::computeChessboardMatches(Mat *imageLeftUndistort, Mat *imageRightUndistort, int patternX, int patternY){
	Size patternsize(patternX, patternY);
	vector<Point2f> cornersLeft, cornersRight;

	//use opencv chessboard detection on the undistorted images
	bool patternfoundLeft = findChessboardCorners(*imageLeftUndistort, patternsize, cornersLeft,
		CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE
		+ CALIB_CB_FAST_CHECK);
	bool patternfoundRight = findChessboardCorners(*imageRightUndistort, patternsize, cornersRight,
		CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE
		+ CALIB_CB_FAST_CHECK);

	vector<vector<Point2f>> result;
	if(patternfoundLeft && patternfoundRight){//both patterns found
		cornerSubPix(*imageLeftUndistort, cornersLeft, Size(11, 11), Size(-1, -1),
		TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
		cornerSubPix(*imageRightUndistort, cornersRight, Size(11, 11), Size(-1, -1),
			TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
		result.push_back(cornersLeft);
		result.push_back(cornersRight);

		vector<DMatch> chessmatches;
		vector<KeyPoint> keypointsleft, keypointsright;
		//save the matches
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

	return result;
}