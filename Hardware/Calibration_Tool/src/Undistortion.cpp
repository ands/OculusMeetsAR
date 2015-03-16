#include "StdAfx.h"
#include "Undistortion.h"


Undistortion::Undistortion(char *calibFile, float sf)
{
	getOcamModel(calibFile);
	mapx = Mat(ocam.height,ocam.width,CV_32FC1);
	mapy = Mat(ocam.height,ocam.width,CV_32FC1);
	calculateLUT(sf);
}

Undistortion::Undistortion(void)
{
}

Undistortion::~Undistortion(void)
{
}

Mat Undistortion::undistort(Mat *image){
	Mat result=Mat(image->rows,image->cols,image->type());
	remap( *image, result, mapx, mapy, INTER_CUBIC,BORDER_TRANSPARENT,0);
	return result;
}


void Undistortion::calculateLUT(float sf)
{
	int i, j;
	int width = mapx.cols;
	int height = mapx.rows;
	float Nxc = height/2.0f;
	float Nyc = width/2.0f;
	float Nz  = -width/sf;
	double M[3];
	double m[2];

	for (i=0; i<height; i++){
		for (j=0; j<width; j++)
		{   
			M[0] = (i - Nxc);
			M[1] = (j - Nyc);
			M[2] = Nz;
			worldToCam(m, M, &ocam);

			mapx.at<float>(i,j) = (float) m[1];
			mapy.at<float>(i,j) = (float) m[0];
		}
	}
}

Rect Undistortion::calculateROI(){
	int roix1,roix2,roiy1,roiy2;
	int math=mapx.rows;
	int matw=mapx.cols;
	for(int i=0;i<matw;i++){
		roix1=i;
		float temp=mapx.at<float>(math/2,i);
		if(temp>0){
			break;
		}
	}
	for(int i=matw-1;i>0;i--){
		roix2=i;
		float temp=mapx.at<float>(math/2,i);
		if(temp<matw){
			break;
		}
	}
	for(int i=0;i<math;i++){
		roiy1=i;
		float temp=mapy.at<float>(i,matw/2);
		if(temp>0){
			break;
		}
	}
	for(int i=math-1;i>0;i--){
		roiy2=i;
		float temp=mapy.at<float>(i,matw/2);
		if(temp<math){
			break;
		}
	}
	Rect roi(roiy1+10,roix1+10,(roiy2-roiy1-10),(roix2-roix1-10));
	return roi;
}

void Undistortion::worldToCam(double point2D[2], double point3D[3], struct ocamModel *myocam_model)
{
	double *invpol     = myocam_model->invpol; 
	double xc          = (myocam_model->xc);
	double yc          = (myocam_model->yc); 
	double c           = (myocam_model->c);
	double d           = (myocam_model->d);
	double e           = (myocam_model->e);
	int    width       = (myocam_model->width);
	int    height      = (myocam_model->height);
	int length_invpol  = (myocam_model->length_invpol);
	double norm        = sqrt(point3D[0]*point3D[0] + point3D[1]*point3D[1]);
	double theta       = atan(point3D[2]/norm);
	double t, t_i;
	double rho, x, y;
	double invnorm;
	int i;

	if (norm != 0) 
	{
		invnorm = 1/norm;
		t  = theta;
		rho = invpol[0];
		t_i = 1;

		for (i = 1; i < length_invpol; i++)
		{
			t_i *= t;
			rho += t_i*invpol[i];
		}

		x = point3D[0]*invnorm*rho;
		y = point3D[1]*invnorm*rho;

		point2D[0] = x*c + y*d + xc;
		point2D[1] = x*e + y   + yc;
	}
	else
	{
		point2D[0] = xc;
		point2D[1] = yc;
	}
}



void Undistortion::getOcamModel(char *filename)
{
	double *pol        = ocam.pol;
	double *invpol     = ocam.invpol; 
	double *xc         = &(ocam.xc);
	double *yc         = &(ocam.yc); 
	double *c          = &(ocam.c);
	double *d          = &(ocam.d);
	double *e          = &(ocam.e);
	int    *width      = &(ocam.width);
	int    *height     = &(ocam.height);
	int *length_pol    = &(ocam.length_pol);
	int *length_invpol = &(ocam.length_invpol);
	FILE *f;
	char buf[CMV_MAX_BUF];
	int i;

	//Open file
	if(fopen_s(&f,filename,"r")!=0)
	{
		printf("File %s cannot be opened\n", filename);
		return;
	}

	//Read polynomial coefficients
	fgets(buf,CMV_MAX_BUF,f);
	fscanf_s(f,"\n");
	fscanf_s(f,"%d", length_pol);
	for (i = 0; i < *length_pol; i++)
	{
		fscanf_s(f," %lf",&pol[i]);
	}

	//Read inverse polynomial coefficients
	fscanf_s(f,"\n");
	fgets(buf,CMV_MAX_BUF,f);
	fscanf_s(f,"\n");
	fscanf_s(f,"%d", length_invpol);
	for (i = 0; i < *length_invpol; i++)
	{
		fscanf_s(f," %lf",&invpol[i]);
	}

	//Read center coordinates
	fscanf_s(f,"\n");
	fgets(buf,CMV_MAX_BUF,f);
	fscanf_s(f,"\n");
	fscanf_s(f,"%lf %lf\n", xc, yc);

	//Read affine coefficients
	fgets(buf,CMV_MAX_BUF,f);
	fscanf_s(f,"\n");
	fscanf_s(f,"%lf %lf %lf\n", c,d,e);

	//Read image size
	fgets(buf,CMV_MAX_BUF,f);
	fscanf_s(f,"\n");
	fscanf_s(f,"%d %d", height, width);

	fclose(f);
}