#include "StdAfx.h"
#include "ImageHandler.h"

ImageHandler::ImageHandler(void)
{
	numOfImages=0;
	leftUndistortion = Undistortion("./calib_results_CAM1.txt",3.0);
	Rect roiLeft=leftUndistortion.calculateROI();
	rightUndistortion = Undistortion("./calib_results_CAM2.txt",3.0);
	Rect roiRight=rightUndistortion.calculateROI();
	matcher = Matching(roiLeft,roiRight);
}

ImageHandler::~ImageHandler(void)
{
}

void ImageHandler::loadImagePairs(vector<const char*> imagepathLeft, vector<const char*> imagepathRight){
	for(int i=0;i<imagepathLeft.size();i++){
		leftImages.push_back(imread(imagepathLeft[i],CV_LOAD_IMAGE_GRAYSCALE));
		rightImages.push_back(imread(imagepathRight[i],CV_LOAD_IMAGE_GRAYSCALE));
	}
	numOfImages=imagepathLeft.size();
}

void ImageHandler::undistortAndRotate(){
	for(int i=0;i<numOfImages;i++){
		leftImages[i]=leftUndistortion.undistort(&(leftImages[i])).t();
		rightImages[i]=rightUndistortion.undistort(&(rightImages[i])).t();
	}
}

void ImageHandler::calculateMatches(int method){
	rightMatch.clear();
	leftMatch.clear();
	for(int i=0;i<numOfImages;i++){
		vector<vector<Point2f>> pairMatches;
		Mat currentLeft = leftImages[i];
		Mat currentRight = rightImages[i];
		if(method==1){//Chessboardmatching
			pairMatches=matcher.computeChessboardMatches(&currentLeft,&currentRight);
		}
		else if(method==2){//SIFT+Ransac Automatching
			pairMatches=matcher.siftMatcher(&currentLeft,&currentRight,0);
		}
		else{//SIFT+Ransac manual selection
			pairMatches=matcher.siftMatcher(&currentLeft,&currentRight,1);
		}
		if(pairMatches.size()!=0){
			for(int j=0;j<pairMatches[0].size();j++){
				//set min distance of matchpairs
				if(leftMatch.size()==0){
					leftMatch.push_back(pairMatches[0][j]);
					rightMatch.push_back(pairMatches[1][j]);
				}
				bool nearby=false;
				for(int k=0;k<leftMatch.size();k++){
					if(dist(leftMatch[k],pairMatches[0][j])<70 && dist(leftMatch[k]-pairMatches[0][j],rightMatch[k]-pairMatches[1][j])<10)
					{
						nearby=true;
						break;
					}
				}
				if(!nearby){
					leftMatch.push_back(pairMatches[0][j]);
					rightMatch.push_back(pairMatches[1][j]);
				}
			}
		}
	}
}

double ImageHandler::dist(Point2f p, Point2f q){
	return sqrt((p.x-q.x)*(p.x-q.x)+(p.y-q.y)*(p.y-q.y));
}


//TODO: HOMOGRAPHIEN NORMALISIEREN
void ImageHandler::estimateGeometry(int method){
	if(method==2){//method after Zilly, Müller, Eisert, Kauff
		epigeo.homographyZMEK(leftMatch,rightMatch);
	}
	else if(method==1){//opencv estimation using Hartley's work 8-point/ransac
		epigeo.homographyHartley(leftMatch,rightMatch);
	}
	std::cout<<"\nEstimation successful.";
	epigeo.homographyError(leftMatch,rightMatch);
}

void ImageHandler::visualize(){
	if(numOfImages==0){
		std::cout<<"\nERROR: No images to visualize epipolar geometry.";
	}
	else if(epigeo.leftHomography.cols==0){
		std::cout<<"\nERROR: No homographies to visualize epipolar geometry.";
	}
	else if(leftMatch.size()==0){
		std::cout<<"\nERROR: No matches to visualize epipolar geometry.";
	}
	else{
		//rectify and save one image pair using the approximated homographies
		Mat imageLeftRectified,imageRightRectified;
		warpPerspective(leftImages[0],imageLeftRectified,epigeo.leftHomography,leftImages[0].size(),1,0,Scalar());
		warpPerspective(rightImages[0],imageRightRectified,epigeo.rightHomography,rightImages[0].size(),1,0,Scalar());
		saveImg(imageLeftRectified,"LeftRectified.jpg");
		saveImg(imageRightRectified,"RightRectified.jpg");

		//draw epipolarlines corresponding to the approximated fundamental matrix F
		drawEpipolarLines("test",epigeo.F,leftImages[0],rightImages[0],leftMatch,rightMatch,5);
		std::cout<<"\nRectified examples and image containing epipolar lines have been saved.";
	}
	std::cout<<"\nPress Enter\n";
	std::cin.ignore();
	std::cin.get();
}

void ImageHandler::saveMatches(){
	std::ofstream leftfile;
	std::remove("leftmatch.txt");
	leftfile.open ("leftmatch.txt");
	for(int i=0;i<leftMatch.size();i++){
		leftfile << leftMatch[i].x<<" "<<leftMatch[i].y<<"\n";
	}
	leftfile.close();

	std::ofstream rightfile;
	std::remove("rightmatch.txt");
	rightfile.open ("rightmatch.txt");
	for(int i=0;i<rightMatch.size();i++){
		rightfile << rightMatch[i].x<<" "<<rightMatch[i].y<<"\n";
	}
	rightfile.close();
}

void ImageHandler::loadMatches(){
	std::ifstream leftfile, rightfile;
	leftMatch.clear();
	rightMatch.clear();
	leftfile.open("leftmatch.txt");
	if (leftfile.is_open())
	{
		string line;
		while (getline(leftfile,line))
		{
			std::string::size_type sz;
			float x = std::stof (line,&sz);
			float y = std::stof (line.substr(sz));
			Point2f currentPoint(x,y);
			leftMatch.push_back(currentPoint);
		}
		leftfile.close();
	}
	rightfile.open("rightmatch.txt");
	if (rightfile.is_open())
	{
		string rline;
		while (getline(rightfile,rline))
		{
			std::string::size_type sz;
			float x = std::stof (rline,&sz);
			float y = std::stof (rline.substr(sz));
			Point2f currentPoint(x,y);
			rightMatch.push_back(currentPoint);
		}
		rightfile.close();
	}
	if(leftMatch.size()!=0&&leftMatch.size()==rightMatch.size()){
		std::cout<<"\nMatches successfully loaded.";
	}
	else{
		std::cout<<"\nUnable to load matches.";
	}
	std::cout<<"\nPress Enter\n";
	std::cin.ignore();
	std::cin.get();
}

void ImageHandler::saveEpiGeo(){
	if(epigeo.saveMatrices("./Fundamental.txt","./homography_CAM1.txt","./homography_CAM2.txt")){
		std::cout<<"\nFiles successfully saved.";
	}
}
void ImageHandler::loadEpiGeo(){
	if(epigeo.loadMatrices("./Fundamental.txt","./homography_CAM1.txt","homography_CAM2.txt")){
		std::cout<<"\nFiles successfully loaded.";
	}
}

void ImageHandler::saveImg(Mat img, string name){
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
	compression_params.push_back(90);
	imwrite(name,img,compression_params);
}

float ImageHandler::distancePointLine(const cv::Point2f point, const Vec<float,3>& line)
{
	//Line is given as a*x + b*y + c = 0
	return std::fabsf(line(0)*point.x + line(1)*point.y + line(2))
		/ std::sqrt(line(0)*line(0)+line(1)*line(1));
}

void ImageHandler::drawEpipolarLines(const std::string& title, const Mat F,
	const cv::Mat& img1, const cv::Mat& img2,
	const std::vector<cv::Point2f> points1,
	const std::vector<cv::Point2f> points2,
	const float inlierDistance)
{
	cv::Mat outImg(img1.rows, img1.cols*2, CV_8UC3);
	cv::Rect rect1(0,0, img1.cols, img1.rows);
	cv::Rect rect2(img1.cols, 0, img1.cols, img1.rows);
	/*
	* Allow color drawing
	*/
	if (img1.type() == CV_8U)
	{
		cv::cvtColor(img1, outImg(rect1), CV_GRAY2BGR);
		cv::cvtColor(img2, outImg(rect2), CV_GRAY2BGR);
	}
	else
	{
		img1.copyTo(outImg(rect1));
		img2.copyTo(outImg(rect2));
	}
	std::vector<cv::Vec<float,3>> epilines1, epilines2;
	cv::computeCorrespondEpilines(points1, 1, F, epilines1); //Index starts with 1
	cv::computeCorrespondEpilines(points2, 2, F, epilines2);

	CV_Assert(points1.size() == points2.size() &&
		points2.size() == epilines1.size() &&
		epilines1.size() == epilines2.size());

	cv::RNG rng(0);
	for(size_t i=0; i<points1.size(); i++)
	{
		if(inlierDistance > 0)
		{
			if(distancePointLine(points1[i], epilines2[i]) > inlierDistance ||
				distancePointLine(points2[i], epilines1[i]) > inlierDistance)
			{
				//The point match is no inlier
				continue;
			}
		}
		/*
		* Epipolar lines of the 1st point set are drawn in the 2nd image and vice-versa
		*/
		cv::Scalar color(rng(256),rng(256),rng(256));

		cv::line(outImg(rect2),
			cv::Point(0,-epilines1[i][2]/epilines1[i][1]),
			cv::Point(img1.cols,-(epilines1[i][2]+epilines1[i][0]*img1.cols)/epilines1[i][1]),
			color);
		cv::circle(outImg(rect1), points1[i], 3, color, -1, CV_AA);

		cv::line(outImg(rect1),
			cv::Point(0,-epilines2[i][2]/epilines2[i][1]),
			cv::Point(img2.cols,-(epilines2[i][2]+epilines2[i][0]*img2.cols)/epilines2[i][1]),
			color);
		cv::circle(outImg(rect2), points2[i], 3, color, -1, CV_AA);
	}
	saveImg(outImg,"epilines.jpg");

	//namedWindow("Epipolar lines",CV_WINDOW_NORMAL);
	imshow("Epipolar lines",outImg);
	//resizeWindow("Epipolar lines",1280,854);
	waitKey(0);
	destroyWindow("Epipolar lines");
}
