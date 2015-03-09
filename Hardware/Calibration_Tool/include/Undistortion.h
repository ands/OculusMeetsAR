#pragma once

#define CMV_MAX_BUF 1024
#define MAX_POL_LENGTH 64

using namespace cv;
//Undistortion class using Davide Scaramuzzas C++ functions (see OcamCalib Toolbox in our documentation)
class Undistortion
{
public:
	struct ocamModel
	{
		double pol[MAX_POL_LENGTH];    // the polynomial coefficients: pol[0] + x"pol[1] + x^2*pol[2] + ... + x^(N-1)*pol[N-1]
		int length_pol;                // length of polynomial
		double invpol[MAX_POL_LENGTH]; // the coefficients of the inverse polynomial
		int length_invpol;             // length of inverse polynomial
		double xc;         // row coordinate of the center
		double yc;         // column coordinate of the center
		double c;          // affine parameter
		double d;          // affine parameter
		double e;          // affine parameter
		int width;         // image width
		int height;        // image height
	};

	Undistortion(char *calibFile, float sf);
	Undistortion(void);
	~Undistortion(void);
	Mat Undistortion::undistort(Mat *image);
	Rect Undistortion::calculateROI();

private:
	//Calculates lookup table for image coordinate according to a previously loaded ocam model
	void Undistortion::calculateLUT(float sf);
	//Applies ocam model to point3D. The resulting image point is point2D
	void Undistortion::worldToCam(double point2D[2], double point3D[3], struct ocamModel *myocam_model);
	//Reads ocam model from file
	void Undistortion::getOcamModel(char *filename);

	ocamModel ocam;
	Mat mapx;//lookuptable for x-coordinates
	Mat mapy;//lookuptable for y-coordinates
};

